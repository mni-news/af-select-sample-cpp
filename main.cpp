#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>

#include <httplib.h>
#include <nlohmann/json.hpp>
#include "websocket_client.hpp"

using json = nlohmann::json;

int main(int argc, char ** argv) {
    httplib::Client cli("https://api.alphaflash.com");

    if (argc < 2){
        std::cout << "Usage: <username> <password>" << std::endl;
        return 1;
    }

    std::cout << "Username: " << argv[1] << std::endl;
    std::string user( argv[1]);
    std::string pass(argv[2]);

    json authRequest = {
            {"username",user},
            {"password",pass},
    };

    httplib::Result result = cli.Post(
            "/api/auth/alphaflash-client/token",authRequest.dump(),"application/json"
            );
    if (result->status != 200){
        std::cout << "Login failed" << std::endl;
        return 1;
    }
    json authResponse = json::parse(result->body);
    std::string token = authResponse["access_token"].get<std::string>();


    //REST API Interaction
    httplib::Result calendarResult = cli.Get(
            "/api/select/calendar/events",
            {{ "Authorization", "Bearer " + token }}
            );

    json calendarPage = json::parse(calendarResult->body);
    json content = calendarPage["content"];

    std::cout  << std::endl << "AlphaFlash Select Release Calendar:" << std::endl;
    for (auto & it : content) {
        std::cout << it << '\n';
    }
    std::cout  << std::endl;


    //Websocket API Interaction
    runClient(
            "wss://select.alphaflash.com/wss",
            token ,
            {"/topic/observations"},
            [](const std::string& message) {

                //TODO - parse stomp message and extract payload

                if (message.size() == 1){
                    std::cout <<"Received HEARTBEAT" << std::endl;
                } else {

                    std::cout << "Received Message: " << std::endl
                              << "[" << message << "]" << std::endl;
                }

            });

    return 0;
}
