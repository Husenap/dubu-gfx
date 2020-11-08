message("-- External Project: Vulkan-Headers")
include(FetchContent)

FetchContent_Declare(
    vulkan_headers
    GIT_REPOSITORY  https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG         v1.2.157
)

FetchContent_MakeAvailable(vulkan_headers)
