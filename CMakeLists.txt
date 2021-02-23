cmake_minimum_required(VERSION 3.0.2)
project(arduino_dev)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## Find catkin macros and libraries
find_package(catkin REQUIRED COMPONENTS
  roscpp
)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(NM REQUIRED libnm)


###################################
## catkin specific configuration ##
###################################
catkin_package(
  INCLUDE_DIRS include
  # LIBRARIES add_lib 
  CATKIN_DEPENDS roscpp
)

###########
## Build ##
###########
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${GLIB_INCLUDE_DIRS}
  ${NM_INCLUDE_DIRS}
)

## add .so library
add_library(WirelessLib src/interfaces_manipulation.cpp)

## Declare a C++ executable
add_executable(test_comm src/test_comm.cpp)
add_executable(proto_test src/protocol_ardu.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(WirelessLib
  ${catkin_LIBRARIES} 
  ${GLIB_LIBRARIES}
  ${NM_LIBRARIES}
)

target_link_libraries(test_comm
  ${catkin_LIBRARIES}
  WirelessLib
)

target_link_libraries(proto_test
  ${catkin_LIBRARIES}
)