cmake_minimum_required(VERSION 3.16)

project(FileTransferProtocol)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(common)

add_executable(sender
    sender/main.cpp
)

add_executable(receiver
    receiver/main.cpp
)

target_link_libraries(sender ws2_32)
target_link_libraries(receiver ws2_32)