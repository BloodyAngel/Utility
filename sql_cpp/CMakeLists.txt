cmake_minimum_required(VERSION 3.5)

project(SQL_CPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)

find_library(LIB_FMT fmt REQUIRED)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -Wall -Wextra")

file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)
file(GLOB_RECURSE SRC_FILES src/*.c src/*.cpp)

add_executable(${PROJECT_NAME} main.cpp ${HEADER_FILES} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} Threads::Threads SQLite::SQLite3 ${LIB_FMT})
target_include_directories(${PROJECT_NAME} PUBLIC include 3rd_party)
