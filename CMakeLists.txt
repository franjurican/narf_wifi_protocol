cmake_minimum_required(VERSION 3.0.2)
project(narf_wifi_protocol)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## custom include
include_directories(client/narf_protocol)

## add .so library
# add_library(NarfWifiProtocolLib client/narf_protocol_client.cpp)

## Declare a C++ executable
add_executable(proto_test client/protocol_ardu.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(proto_test 
  #NarfWifiProtocolLib
)