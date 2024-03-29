cmake_minimum_required(VERSION 3.0.0)

# Define LINUX
if (UNIX AND NOT APPLE)
    set(LINUX 1)
endif()

# Setup modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Some compiler flags
set(CMAKE_CXX_STANDARD 17) # C++17
if (MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/MP /FS")
endif()

# Project name
project(2023_3)

# Define _DEBUG
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()

#justwindowsthings
if (WIN32)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Project files
file(GLOB srcfiles ./src/*.*)
list(APPEND includes PUBLIC ./src/)

# .exe, use WinMain on Windows
source_group("thirdparty" FILES ${srcthirdparty})
source_group("src" FILES ${srcfiles})
add_executable(${PROJECT_NAME} WIN32 ${srcfiles} ${srcthirdparty})

# Work dir
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

# Lib/Headers
target_include_directories(${PROJECT_NAME} PUBLIC ${includes})
target_link_libraries(${PROJECT_NAME} PUBLIC ${libs})
