cmake_minimum_required(VERSION 3.0.0)

project(AdventOfCode)

# Onut
add_subdirectory(./thirdparty/onut onut)
list(APPEND libs PUBLIC libonut)
list(APPEND includes PUBLIC ./thirdparty/onut/include/)

add_subdirectory(./2015/1)
add_subdirectory(./2015/2)
add_subdirectory(./2015/3)
add_subdirectory(./2015/4)
add_subdirectory(./2015/5)
add_subdirectory(./2015/6)

add_subdirectory(./2020/1)
add_subdirectory(./2020/2)
add_subdirectory(./2020/3)
add_subdirectory(./2020/4)
add_subdirectory(./2020/5)
add_subdirectory(./2020/6)
add_subdirectory(./2020/7)
add_subdirectory(./2020/8)
add_subdirectory(./2020/9)
add_subdirectory(./2020/10)
