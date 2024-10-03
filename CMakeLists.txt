cmake_minimum_required(VERSION 3.27)
project(SPRINT9_FINALE)

set(CMAKE_CXX_STANDARD 17)

add_executable(SPRINT9_FINALE main.cpp
        transport_catalogue.cpp
        stat_reader.cpp
        input_reader.cpp)
