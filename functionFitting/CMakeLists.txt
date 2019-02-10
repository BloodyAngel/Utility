cmake_minimum_required(VERSION 3.1)
project(FunctionFitting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB HEADER_FILES *.h *.hpp)
add_executable(${PROJECT_NAME} example.cpp ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC .)
