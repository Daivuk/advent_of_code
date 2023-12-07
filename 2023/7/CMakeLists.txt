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

get_filename_component(year_folder "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
get_filename_component(PROBLEM_YEAR ${year_folder} NAME)
get_filename_component(PROBLEM_ID ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Project name
project("${PROBLEM_YEAR}_${PROBLEM_ID}")
add_definitions(-DPROBLEM_ID="${PROBLEM_ID}")
add_definitions(-DPROBLEM_YEAR="${PROBLEM_YEAR}")

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
file(GLOB mainfiles ../../src/*.*)
file(GLOB srcfiles ./src/*.*)
list(APPEND includes PUBLIC ./src/)
list(APPEND includes PUBLIC ../../src/)

# .exe, use WinMain on Windows
source_group("main" FILES ${mainfiles})
source_group("src" FILES ${srcfiles})
source_group("thirdparty" FILES ${srcthirdparty})
add_executable(${PROJECT_NAME} WIN32 ${mainfiles} ${srcfiles} ${srcthirdparty})

# Work dir
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

# Lib/Headers
target_include_directories(${PROJECT_NAME} PUBLIC ${includes})
target_link_libraries(${PROJECT_NAME} PUBLIC ${libs})
