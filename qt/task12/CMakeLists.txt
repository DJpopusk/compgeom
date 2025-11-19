add_executable(task12_visual
    main.cpp
)

target_link_libraries(task12_visual
    PRIVATE
        polybool_common
)

set_target_properties(task12_visual PROPERTIES OUTPUT_NAME "task12_visual")
