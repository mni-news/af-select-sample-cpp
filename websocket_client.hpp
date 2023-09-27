
/**
 * Runs websocket stomp client
 * @param uri - the websocket URI
 * @param token - the auth token
 * @param destinations - a list of destinations to subscribe to
 * @param fp - a callback for messages
 */
void runClient(
        const std::string & uri,
        const std::string & token,
        const std::vector<std::string> & destinations,
        void(*fp)(const std::string&)
        );