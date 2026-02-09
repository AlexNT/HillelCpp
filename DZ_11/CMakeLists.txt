cmake_minimum_required(VERSION 3.16)

project(boost_tcp_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT BOOST_INCLUDEDIR)
  message(FATAL_ERROR "BOOST_INCLUDEDIR is not set. Pass -DBOOST_INCLUDEDIR=...")
endif()

add_executable(tcp_server server.cpp)
target_include_directories(tcp_server PRIVATE "${BOOST_INCLUDEDIR}")
target_compile_definitions(tcp_server PRIVATE BOOST_ERROR_CODE_HEADER_ONLY BOOST_SYSTEM_NO_DEPRECATED)

add_executable(tcp_client client.cpp)
target_include_directories(tcp_client PRIVATE "${BOOST_INCLUDEDIR}")
target_compile_definitions(tcp_client PRIVATE BOOST_ERROR_CODE_HEADER_ONLY BOOST_SYSTEM_NO_DEPRECATED)

