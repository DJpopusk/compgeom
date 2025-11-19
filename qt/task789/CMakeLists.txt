set(TASK789_SOURCES
    main.cpp
    mainwindow.cpp
    canvas_model.cpp
    canvas_widget.cpp
)

add_executable(task789_visual ${TASK789_SOURCES})

target_link_libraries(task789_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task789_algo
)

target_include_directories(task789_visual
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(task789_visual PROPERTIES
    OUTPUT_NAME "task789_visual"
)
