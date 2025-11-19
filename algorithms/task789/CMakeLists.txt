add_library(task789_algo STATIC
    src/convex_boolean.cpp
)

target_include_directories(task789_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(task789_algo PUBLIC cxx_std_17)

add_library(compgeom::task789_algo ALIAS task789_algo)
