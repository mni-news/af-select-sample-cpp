# AlphaFlash Select C++ Sample

This code is for demo purposes only.

## Required Tools

* A C++ Compiler that supports C++11
* CMake >= 3.20
* The Boost library
* Openssl library

## Build and Dependencies

The build file is in CMakeLists.txt. 

It uses the following dependencies which are downloaded via the `FetchContent` CMake module:

| Library | Description | Link 
| --- | --- | ---
| nlohmann/json | A library for processing JSON | https://github.com/nlohmann/json 
| cpp-httplib | A lightweight http client | https://github.com/yhirose/cpp-httplib
| websocketpp | A websocket client | https://github.com/zaphoyd/websocketpp

## About the code
* [main.cpp](main.cpp)  contains samples of auth and REST API usage
* [websocket_client.cpp](websocket_client.cpp) contains a primative websocket stomp client. This is not 
production ready, and would need refinement for actual usage. 