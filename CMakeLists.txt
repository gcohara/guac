cmake_minimum_required(VERSION 3.14.5)

project(guac)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_HOME_DIRECTORY}/bin
    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g") 

add_executable(guac ${SOURCES})
