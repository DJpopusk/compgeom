add_library(task3_algo STATIC
    src/point_segment_ld.cpp
)

target_include_directories(task3_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(task3_algo PUBLIC cxx_std_17)

add_library(compgeom::task3_algo ALIAS task3_algo)
