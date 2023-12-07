cmake_minimum_required(VERSION 3.0.0)

project(AdventOfCode)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Onut
add_subdirectory(./thirdparty/onut onut)
list(APPEND libs PUBLIC libonut)
list(APPEND includes PUBLIC ./thirdparty/onut/include/)

# Projects
FUNCTION(add_year year)
    file(GLOB days "./${year}/*")
    FOREACH(day ${days})
        add_subdirectory("${day}")
        get_filename_component(PROBLEM_ID ${day} NAME)
        set_target_properties("${year}_${PROBLEM_ID}" PROPERTIES FOLDER "${year}")
    ENDFOREACH()
ENDFUNCTION()

add_year(2015)
add_year(2020)
add_year(2021)
add_year(2023)
