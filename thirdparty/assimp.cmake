message("-- External Project: assimp")
include(FetchContent)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY  https://github.com/assimp/assimp.git
    GIT_TAG         v5.0.1
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

set(assimp_libraries
    "assimp"
    "zlib"
    "zlibstatic"
    "IrrXML"
    "UpdateAssimpLibsDebugSymbolsAndDLLs")

foreach(library ${assimp_libraries})
    set_target_properties(${library} PROPERTIES FOLDER "thirdparty/assimp")
endforeach()