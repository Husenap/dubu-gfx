# Vulkan-Headers
message("-- External Project: Vulkan-Headers")
include(FetchContent)

FetchContent_Declare(
    vulkan_headers
    GIT_REPOSITORY  https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG         v1.2.157
)

FetchContent_MakeAvailable(vulkan_headers)

# Vulkan-Loader
message("-- External Project: Vulkan-Loader")
include(FetchContent)

FetchContent_Declare(
    vulkan_loader
    GIT_REPOSITORY  https://github.com/KhronosGroup/Vulkan-Loader.git
    GIT_TAG         v1.2.157
)

FetchContent_MakeAvailable(vulkan_loader)

if(MSVC)
    set_target_properties("asm_offset" PROPERTIES FOLDER "thirdparty/vulkan")
    set_target_properties("loader-norm" PROPERTIES FOLDER "thirdparty/vulkan")
    set_target_properties("loader-opt" PROPERTIES FOLDER "thirdparty/vulkan")
    set_target_properties("loader-unknown-chain" PROPERTIES FOLDER "thirdparty/vulkan")
    set_target_properties("vulkan" PROPERTIES FOLDER "thirdparty/vulkan")
    set_target_properties("VulkanLoader_generated_source" PROPERTIES FOLDER "thirdparty/vulkan")
    set_target_properties("loader_asm_gen_files" PROPERTIES FOLDER "thirdparty/vulkan")
endif()
