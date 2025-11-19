add_library(polybool_common STATIC
    canvas_model.cpp
    canvas_widget.cpp
    main_window.cpp
)

target_include_directories(polybool_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(polybool_common
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task10_algo
)
