cmake_minimum_required(VERSION 3.27)
project(os_hw10)

set(CMAKE_CXX_STANDARD 17)

add_executable(server server.cpp)
add_executable(sender sender.cpp Client.cpp)
add_executable(receiver receiver.cpp Client.cpp)
