set(TASK4_SOURCES
    main.cpp
    mainwindow.cpp
    canvas_model.cpp
    canvas_widget.cpp
)

add_executable(task4_visual ${TASK4_SOURCES})

target_link_libraries(task4_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task4_algo
)

target_include_directories(task4_visual
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(task4_visual PROPERTIES
    OUTPUT_NAME "task4_visual"
)
