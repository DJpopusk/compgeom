add_library(task5_algo STATIC
    src/delaunay.cpp
)

target_include_directories(task5_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(task5_algo PUBLIC cxx_std_17)

add_library(compgeom::task5_algo ALIAS task5_algo)
