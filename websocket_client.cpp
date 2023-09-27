

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/connection.hpp>

#include <iostream>
#include <utility>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;
using websocketpp::lib::bind;


context_ptr on_tls_init() {
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);

        // TODO - this disables verification, replace with real code
        ctx->set_verify_mode(boost::asio::ssl::verify_none);
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return ctx;
}

std::string stomp_message(std::string type, const std::vector<std::vector<std::string>>&  headers){

    std::string msg =  std::move(type);
    msg = msg.append("\r\n");

    for (std::vector<std::string> header : headers) {
        msg = msg.append(header[0]);
        msg = msg.append(":");
        msg = msg.append(header[1]);
        msg = msg.append("\r\n");
    }
    msg = msg.append("\r\n");
    msg = msg.append("\0",1);

    return msg;
}

void run_client(
        const std::string& uri,
        const std::string & token,
        const std::vector<std::string> & destinations,
        void(*handler)(const std::string&)){
    client c;

    try {

        // configure websocketpp logging
        c.set_error_channels(websocketpp::log::elevel::all);
        c.clear_access_channels((websocketpp::log::elevel::all));
        c.set_access_channels(websocketpp::log::alevel::app);

        // Initialize ASIO
        c.init_asio();

        // on open connection
        c.set_open_handler([&c, &token](websocketpp::connection_hdl hdl) {
            websocketpp::lib::error_code ec;

            c.get_alog().write(websocketpp::log::alevel::app, "Connection established, sending credentials");

            c.send(
                std::move(hdl),
               stomp_message("CONNECT",{{"passcode",token},{"heart-beat","0,30000"}}),
               websocketpp::frame::opcode::text, ec
               );

        });

        // on websocket message
        c.set_message_handler([&c, &destinations, &handler](const websocketpp::connection_hdl& hdl,
                                 const websocketpp::connection<websocketpp::config::asio_tls_client>::message_ptr& msg) {

            if (msg->get_payload().size() == 1){
                c.get_alog().write(websocketpp::log::alevel::app, "Received HEARTBEAT ");
            } else if (msg->get_payload().rfind("CONNECTED",0) == 0){

                for (std::string destination: destinations) {

                    c.get_alog().write(websocketpp::log::alevel::app, "Connection successful, subscribing to: " + destination);

                    websocketpp::lib::error_code ec;
                    c.send(
                            hdl,
                           stomp_message("SUBSCRIBE",{{"destination",destination}}),
                           websocketpp::frame::opcode::text,
                           ec
                    );
                }

            } else {
                handler(msg->get_payload());
            }
        });

        // on TLS
        c.set_tls_init_handler(bind(&on_tls_init));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        c.get_alog().write(websocketpp::log::alevel::app, "Connecting to " + uri);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();

        c.get_alog().write(websocketpp::log::alevel::app, "Running client");
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}

