cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CMAKE_POSITION_INDEPENDENT_CODE True)

project(parsing VERSION 0.1.1 LANGUAGES CXX)

# add_library("${PROJECT_NAME}_static" STATIC src/utils.cpp src/action.cpp src/actiongroup.cpp src/argumentparser.cpp)
# target_include_directories("${PROJECT_NAME}_static" PUBLIC include)

# add_library("${PROJECT_NAME}_shared" SHARED src/utils.cpp src/action.cpp src/actiongroup.cpp src/argumentparser.cpp)
# target_include_directories("${PROJECT_NAME}_shared" PUBLIC include)

# add_library("${PROJECT_NAME}_object" OBJECT src/utils.cpp src/action.cpp src/actiongroup.cpp src/argumentparser.cpp)
# target_include_directories("${PROJECT_NAME}_object" PUBLIC include)

add_library(${PROJECT_NAME} STATIC src/utils.cpp src/action.cpp src/actiongroup.cpp src/argumentparser.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_executable(hacky EXCLUDE_FROM_ALL src/main.cpp)
target_link_libraries(hacky PRIVATE ${PROJECT_NAME})
