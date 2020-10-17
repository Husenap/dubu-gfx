set(target_name "dubu_gfx")

set(src_precompiled
    "src/dubu_gfx/precompiled.h")

set(src_vulkan
    "src/dubu_gfx/vulkan/Device.cpp"
    "src/dubu_gfx/vulkan/Device.h"
    "src/dubu_gfx/vulkan/Surface.cpp"
    "src/dubu_gfx/vulkan/Surface.h"
    "src/dubu_gfx/vulkan/Instance.cpp"
    "src/dubu_gfx/vulkan/Instance.h"
    "src/dubu_gfx/vulkan/QueueFamilyIndices.cpp"
    "src/dubu_gfx/vulkan/QueueFamilyIndices.h")

set(src_dubu_gfx
    "src/dubu_gfx/dubu_gfx.h")

set(src_files
    ${src_precompiled}
    ${src_vulkan}
    ${src_dubu_gfx})

# Project
add_library(${target_name} STATIC ${src_files})
set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${target_name}
    compiler_features
    compiler_warnings
    Vulkan::Vulkan)

target_precompile_headers(${target_name} PUBLIC ${src_precompiled})
target_include_directories( ${target_name} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

source_group("src" FILES ${src_dubu_gfx})
source_group("src/vulkan" FILES ${src_vulkan})
source_group("src/precompiled" FILES ${src_precompiled})
source_group("src/precompiled" REGULAR_EXPRESSION "cmake_pch\.[ch]xx")

set_target_properties(${target_name} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
