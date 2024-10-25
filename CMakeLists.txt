cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(parsing VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS BASE_DIRS include FILES include/parsing.hpp)

add_executable(grr src/main.cpp)
target_link_libraries(grr parsing)
