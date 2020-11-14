set(target_name "dubu_gfx")

set(src_precompiled
    "src/dubu_gfx/precompiled.h")

set(src_vulkan
    "src/dubu_gfx/vulkan/Buffer.cpp"
    "src/dubu_gfx/vulkan/Buffer.h"
    "src/dubu_gfx/vulkan/CommandBuffer.cpp"
    "src/dubu_gfx/vulkan/CommandBuffer.h"
    "src/dubu_gfx/vulkan/CommandPool.cpp"
    "src/dubu_gfx/vulkan/CommandPool.h"
    "src/dubu_gfx/vulkan/DescriptorPool.cpp"
    "src/dubu_gfx/vulkan/DescriptorPool.h"
    "src/dubu_gfx/vulkan/DescriptorSet.cpp"
    "src/dubu_gfx/vulkan/DescriptorSet.h"
    "src/dubu_gfx/vulkan/DescriptorSetLayout.cpp"
    "src/dubu_gfx/vulkan/DescriptorSetLayout.h"
    "src/dubu_gfx/vulkan/Device.cpp"
    "src/dubu_gfx/vulkan/Device.h"
    "src/dubu_gfx/vulkan/DeviceMemory.cpp"
    "src/dubu_gfx/vulkan/DeviceMemory.h"
    "src/dubu_gfx/vulkan/DrawingCommand.cpp"
    "src/dubu_gfx/vulkan/DrawingCommand.h"
    "src/dubu_gfx/vulkan/DrawingCommands.h"
    "src/dubu_gfx/vulkan/DynamicState.cpp"
    "src/dubu_gfx/vulkan/DynamicState.h"
    "src/dubu_gfx/vulkan/Framebuffer.cpp"
    "src/dubu_gfx/vulkan/Framebuffer.h"
    "src/dubu_gfx/vulkan/GraphicsPipeline.cpp"
    "src/dubu_gfx/vulkan/GraphicsPipeline.h"
    "src/dubu_gfx/vulkan/Image.cpp"
    "src/dubu_gfx/vulkan/Image.h"
    "src/dubu_gfx/vulkan/Instance.cpp"
    "src/dubu_gfx/vulkan/Instance.h"
    "src/dubu_gfx/vulkan/QueueFamilyIndices.cpp"
    "src/dubu_gfx/vulkan/QueueFamilyIndices.h"
    "src/dubu_gfx/vulkan/RenderPass.cpp"
    "src/dubu_gfx/vulkan/RenderPass.h"
    "src/dubu_gfx/vulkan/Sampler.cpp"
    "src/dubu_gfx/vulkan/Sampler.h"
    "src/dubu_gfx/vulkan/Shader.cpp"
    "src/dubu_gfx/vulkan/Shader.h"
    "src/dubu_gfx/vulkan/ShaderModule.cpp"
    "src/dubu_gfx/vulkan/ShaderModule.h"
    "src/dubu_gfx/vulkan/Surface.cpp"
    "src/dubu_gfx/vulkan/Surface.h"
    "src/dubu_gfx/vulkan/Swapchain.cpp"
    "src/dubu_gfx/vulkan/Swapchain.h"
    "src/dubu_gfx/vulkan/ViewportState.cpp"
    "src/dubu_gfx/vulkan/ViewportState.h")

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
    dubu_log
    Vulkan::Vulkan)

target_precompile_headers(${target_name} PUBLIC ${src_precompiled})
target_include_directories( ${target_name} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

source_group("src" FILES ${src_dubu_gfx})
source_group("src/vulkan" FILES ${src_vulkan})
source_group("src/precompiled" FILES ${src_precompiled})
source_group("src/precompiled" REGULAR_EXPRESSION "cmake_pch\.[ch]xx")

set_target_properties(${target_name} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})