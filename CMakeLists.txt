cmake_minimum_required(VERSION 3.10)
project(kek)

set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ".")

add_executable(main
	logistics.cpp
	classLib.cpp
)