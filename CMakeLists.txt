cmake_minimum_required(VERSION 3.18.2)

project(guac)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_HOME_DIRECTORY}/bin
    )

set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g")


add_executable(guac ${SOURCES})
