set(target_name "dubu_vk_test")

set(src_files
    "src/main.cpp")

add_executable(${target_name} ${src_files})

target_link_libraries(${target_name}
    dubu_window
    dubu_vk)

set_target_properties(${target_name} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER}/test)