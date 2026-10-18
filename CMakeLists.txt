cmake_minimum_required(VERSION 3.12)
project(Nexus_ODEP_Suite VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -O3)
endif()

# Bindet die Unterverzeichnisse in der richtigen Reihenfolge ein
add_subdirectory(ODEP_Core)
add_subdirectory(ODEP_Node)
add_subdirectory(ODEP_Orchestrator)
add_subdirectory(ODEP_Controller)
