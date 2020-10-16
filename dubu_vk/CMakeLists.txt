set(target_name "dubu_vk")

set(src_precompiled
    "src/dubu_vk/precompiled.h")

set(src_vulkan
    "src/dubu_vk/vulkan/Framework.cpp"
    "src/dubu_vk/vulkan/Framework.h")

set(src_dubu_vk
    "src/dubu_vk/dubu_vk.h")

set(src_files
    ${src_precompiled}
    ${src_vulkan}
    ${src_dubu_vk})

# Project
add_library(${target_name} STATIC ${src_files})
set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${target_name}
    compiler_features
    compiler_warnings
    Vulkan::Vulkan)

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_precompile_headers(${target_name} PUBLIC ${src_precompiled})

source_group("src" FILES ${src_dubu_vk})
source_group("src/vulkan" FILES ${src_vulkan})
source_group("src/precompiled" FILES ${src_precompiled})
source_group("src/precompiled" REGULAR_EXPRESSION "cmake_pch\.[ch]xx")

set_target_properties(${target_name} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
