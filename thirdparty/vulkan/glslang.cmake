message("-- External Project: glslang")
include(FetchContent)

FetchContent_Declare(
    glslang
    GIT_REPOSITORY  https://github.com/KhronosGroup/glslang.git
    GIT_TAG         8.13.3743
)

set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glslang)

if(MSVC)
    set_target_properties("glslang" PROPERTIES FOLDER "thirdparty/glslang")
    set_target_properties("OGLCompiler" PROPERTIES FOLDER "thirdparty/glslang")
    set_target_properties("OSDependent" PROPERTIES FOLDER "thirdparty/glslang")
    set_target_properties("SPIRV" PROPERTIES FOLDER "thirdparty/glslang")
    set_target_properties("SPVRemapper" PROPERTIES FOLDER "thirdparty/glslang")
endif()
