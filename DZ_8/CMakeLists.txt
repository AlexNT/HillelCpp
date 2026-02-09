cmake_minimum_required(VERSION 3.8...3.31)
project(DZ_8)

find_package(Boost REQUIRED)
add_executable(DZ_8 example.cpp)
target_link_libraries(DZ_8 Boost::headers)
