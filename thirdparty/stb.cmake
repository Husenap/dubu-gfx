message("-- External Project: stb")
include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY  https://github.com/nothings/stb.git
    GIT_TAG         b42009b
    SOURCE_DIR      "${FETCHCONTENT_BASE_DIR}/stb/stb"
)

FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)

    add_library(stb INTERFACE)

    target_include_directories(stb INTERFACE
        "${stb_SOURCE_DIR}"
        "${stb_SOURCE_DIR}/..")
endif()
