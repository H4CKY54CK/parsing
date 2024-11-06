cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(parsing VERSION 0.1.1 LANGUAGES CXX)

add_library("${PROJECT_NAME}" STATIC src/utils.cpp src/action.cpp src/actiongroup.cpp src/argumentparser.cpp)
target_include_directories("${PROJECT_NAME}" PUBLIC include)

add_executable("${PROJECT_NAME}-test" EXCLUDE_FROM_ALL tests/main.cpp)
target_link_libraries("${PROJECT_NAME}-test" PRIVATE "${PROJECT_NAME}")
