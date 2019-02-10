cmake_minimum_required(VERSION 3.1)
project(Threadpool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads)

file(GLOB HEADER_FILES *.h *.hpp)
add_executable(${PROJECT_NAME} example.cpp ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
