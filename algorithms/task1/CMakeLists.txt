add_library(task1_algo STATIC
    src/point_segment.cpp
)

target_include_directories(task1_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(task1_algo PUBLIC cxx_std_17)

add_library(compgeom::task1_algo ALIAS task1_algo)
