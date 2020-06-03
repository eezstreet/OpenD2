# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARY
#

include(FindPackageHandleStandardArgs)

find_package(OpenGL REQUIRED)


find_path(GLEW_INCLUDE_DIR GL/glew.h
        HINTS ENV
        PATHS include
        DOC "The directory where GL/glew.h resides"
        )

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH x64)
else()
    set(ARCH x86)
endif()

find_library(GLEW_LIBRARY
        NAMES glew GLEW glew32.lib
        HINTS ENV GLEWDIR
        PATH_SUFFIXES Release/${ARCH}
        DOC "The GLEW library")

find_library(GLEW_LIBRARY_STATIC
        NAMES glew32s
        HINTS ENV GLEWDIR
        PATH_SUFFIXES Release/${ARCH}
        DOC "The GLEW library")

# Read version from GL/glew.h file
if(EXISTS "${GLEW_INCLUDE_DIR}/GL/glew.h")
    file(STRINGS "${GLEW_INCLUDE_DIR}/GL/glew.h" _contents REGEX "^VERSION_.+ [0-9]+")
    if(_contents)
        string(REGEX REPLACE ".*VERSION_MAJOR[ \t]+([0-9]+).*" "\\1" GLEW_VERSION_MAJOR "${_contents}")
        string(REGEX REPLACE ".*VERSION_MINOR[ \t]+([0-9]+).*" "\\1" GLEW_VERSION_MINOR "${_contents}")
        string(REGEX REPLACE ".*VERSION_MICRO[ \t]+([0-9]+).*" "\\1" GLEW_VERSION_MICRO "${_contents}")
        set(GLEW_VERSION "${GLEW_VERSION_MAJOR}.${GLEW_VERSION_MINOR}.${GLEW_VERSION_MICRO}")
    endif()
endif()

add_library(GLEW::GLEW SHARED IMPORTED)
set_target_properties(GLEW::GLEW PROPERTIES
        IMPORTED_LOCATION "${GLEW_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}")
target_compile_definitions(GLEW::GLEW INTERFACE GLFW_DLL)
target_link_libraries(GLEW::GLEW INTERFACE opengl32)
target_link_libraries(GLEW::GLEW INTERFACE OpenGL::GL)


add_library(GLEW::Static STATIC IMPORTED)
set_target_properties(GLEW::Static PROPERTIES
        IMPORTED_LOCATION "${GLEW_LIBRARY_STATIC}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}")
target_compile_definitions(GLEW::Static INTERFACE GLEW_STATIC)
target_link_libraries(GLEW::Static INTERFACE OpenGL::GL)

find_package_handle_standard_args(GLEW REQUIRED_VARS
        GLEW_LIBRARY
        GLEW_LIBRARY_STATIC
        GLEW_INCLUDE_DIR
        VERSION_VAR GLEW_VERSION)
