cmake_minimum_required(VERSION 3.20)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(http URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.14.0.tar.gz)
FetchContent_MakeAvailable(http)

FetchContent_Declare(websocket URL https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.tar.gz)
FetchContent_MakeAvailable(websocket)

project(af_select_sample_cpp)

set(CMAKE_CXX_STANDARD 11)

add_executable(af_select_sample_cpp main.cpp websocket_client.cpp)

include_directories(${http_SOURCE_DIR})
include_directories(${json_SOURCE_DIR}/include)
include_directories(${websocket_SOURCE_DIR})


target_link_libraries(af_select_sample_cpp PRIVATE nlohmann_json::nlohmann_json ssl crypto  )
