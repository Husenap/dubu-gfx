set(target_name "dubu_gfx_test")

set(src_files
    "src/Application.cpp"
    "src/Application.h"
    "src/main.cpp")

add_executable(${target_name} ${src_files})

target_link_libraries(${target_name}
    dubu_window
    dubu_gfx)

set_target_properties(${target_name} PROPERTIES
    FOLDER ${${PROJECT_NAME}_FOLDER}/test
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${target_name}>)

target_compile_definitions(${target_name} PRIVATE GLFW_INCLUDE_VULKAN)

add_custom_command(
    TARGET ${target_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/
    $<TARGET_FILE_DIR:${target_name}>/assets/)