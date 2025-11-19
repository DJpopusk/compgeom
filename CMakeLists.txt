cmake_minimum_required(VERSION 3.16)
project(compgeom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(algorithms)

option(COMPGEOM_BUILD_QT "Build Qt visualizers" ON)
if (COMPGEOM_BUILD_QT)
    add_subdirectory(qt)
endif()
