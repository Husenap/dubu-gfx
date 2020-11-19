cmake_minimum_required(VERSION 3.16)

project("dubu_gfx" C CXX)

include("cmake/prevent_in_source_builds.cmake")
include("cmake/standard_project_setup.cmake")

include("cmake/compiler_features.cmake")
include("cmake/compiler_warnings.cmake")

set(${PROJECT_NAME}_FOLDER
    ${PROJECT_NAME}
    CACHE STRING "The folder that ${PROJECT_NAME} projects will be placed in.")

option(${PROJECT_NAME}_BUILD_TESTS
    "If the ${PROJECT_NAME} tests are built in addition to the ${PROJECT_NAME} library."
    ON)

include("thirdparty/vulkan.cmake")
include("thirdparty/dubu_log.cmake")
add_subdirectory("dubu_gfx")

if(${${PROJECT_NAME}_BUILD_TESTS})
    include("thirdparty/dubu_window.cmake")
    include("thirdparty/imgui.cmake")
    include("thirdparty/glm.cmake")
    include("thirdparty/stb.cmake")
    include("thirdparty/assimp.cmake")
    add_subdirectory("dubu_gfx_test")
endif()

if(TARGET uninstall)
    set_target_properties("uninstall" PROPERTIES FOLDER "predefined")
endif()