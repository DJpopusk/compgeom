add_executable(task12_visual
    main.cpp
    canvas_model.cpp
    canvas_widget.cpp
    main_window.cpp
)

target_link_libraries(task12_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task12_algo
)

set_target_properties(task12_visual PROPERTIES OUTPUT_NAME "task12_visual")
