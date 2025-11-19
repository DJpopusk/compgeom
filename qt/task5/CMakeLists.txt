set(TASK5_SOURCES
    main.cpp
    mainwindow.cpp
    canvas_model.cpp
    canvas_widget.cpp
)

add_executable(task5_visual ${TASK5_SOURCES})

target_link_libraries(task5_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task5_algo
)

target_include_directories(task5_visual
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(task5_visual PROPERTIES
    OUTPUT_NAME "task5_visual"
)
