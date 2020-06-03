cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 14)
project(OpenD2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH thirdparty thirdparty/include)
    find_package(GLEW REQUIRED)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif()
else()
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/thirdparty)
    include(ExternalProject)
    include(CMakeFindFrameworks)

    find_package(OpenGL REQUIRED)

    set(TP_OUT ${CMAKE_BINARY_DIR}/out/thirdparty)

    ExternalProject_Add(GLEW_ext
            PREFIX          thirdparty
            URL             https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip
            SOURCE_SUBDIR   build/cmake
            CMAKE_ARGS      -DBUILD_UTILS=OFF -DCMAKE_INSTALL_PREFIX=${TP_OUT}
            )
    add_library(GLEW::Static STATIC IMPORTED)
    set_target_properties(GLEW::Static PROPERTIES
            IMPORTED_LOCATION "${TP_OUT}/lib/libGLEW${CMAKE_STATIC_LIBRARY_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES "${TP_OUT}/include")
    target_link_libraries(GLEW::Static INTERFACE OpenGL::GL)

endif()


find_package(SDL2_mixer REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(GLUT REQUIRED)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)


add_subdirectory(Engine)
add_subdirectory(Modcode)
add_subdirectory(Shared)
add_subdirectory(Libraries)
