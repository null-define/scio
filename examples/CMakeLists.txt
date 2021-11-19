cmake_minimum_required(VERSION 2.8)

include(../cmake/third_party.cmake)

include_directories(../src)

add_executable(client_test client.cc)
target_link_libraries(client_test PUBLIC scio::third_party)
