cmake_minimum_required(VERSION 3.10)
project(kek)

set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ".")

add_executable(logistics
	src/logistics.cpp
	src/login.cpp
	src/classLib.cpp
	src/utils.cpp
	src/StoresAPI.cpp
	src/LogisticsCore.cpp
	src/SyncAPI.cpp
)

add_executable(shop
	src/shop.cpp
	src/login.cpp
	src/classLib.cpp
	src/utils.cpp
	src/StoresAPI.cpp
	src/LogisticsCore.cpp
	src/SyncAPI.cpp
)