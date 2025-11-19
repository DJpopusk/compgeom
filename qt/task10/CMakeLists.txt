add_executable(task10_visual
    main.cpp
)

target_link_libraries(task10_visual
    PRIVATE
        polybool_common
)

set_target_properties(task10_visual PROPERTIES OUTPUT_NAME "task10_visual")
