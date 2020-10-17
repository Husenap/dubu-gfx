message("-- External Project: Vulkan-Loader")
include(FetchContent)

FetchContent_Declare(
    vulkan_loader
    GIT_REPOSITORY  https://github.com/KhronosGroup/Vulkan-Loader.git
    GIT_TAG         v1.2.157
)

FetchContent_MakeAvailable(vulkan_loader)