cmake_minimum_required(VERSION 3.28)
project(WeatherApp)

set(CMAKE_CXX_STANDARD 17)

add_executable(WeatherApp main.cpp)

find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(WeatherApp PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)