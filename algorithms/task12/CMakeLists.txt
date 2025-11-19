add_library(task12_algo INTERFACE)

target_link_libraries(task12_algo
    INTERFACE
        compgeom::task10_algo
)

add_library(compgeom::task12_algo ALIAS task12_algo)
