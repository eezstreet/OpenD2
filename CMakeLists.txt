cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

project(OpenD2)

option(BUILD_GAME "Build Executable" ON)
option(BUILD_D2CLIENT "Build D2Client" ON)
option(BUILD_D2SERVER "Build D2Server" ON)

# Common options
if(WIN32)
	set(STATIC_LIBRARIES dbghelp.lib psapi.lib)
else()
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_COMPILER g++)
endif()

# Add shared files
file(GLOB_RECURSE SHARED_SRC Shared/*.h Shared/*.hpp Shared/*.c Shared/*.cpp)
source_group("Shared" FILES ${SHARED_SRC})

# Build game.exe
if(BUILD_GAME)
	message(STATUS "Including game.exe files")

	file(GLOB_RECURSE ENGINE_SRC Engine/*.h Engine/*.hpp Engine/*.c Engine/*.cpp)

	# Use the included SDL on Windows, use find_package to find SDL on Linux
	if(WIN32)
		list(REMOVE_ITEM ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Platform_Linux.cpp)
		file(GLOB_RECURSE SDL_SRC Libraries/sdl/*)
		file(GLOB_RECURSE ADPCM_SRC Libraries/adpcm/*)
		file(GLOB_RECURSE HUFFMAN_SRC Libraries/huffman/*)
		file(GLOB_RECURSE PKWARE_SRC Libraries/pkware/*)
		file(GLOB_RECURSE GLEW_SRC Libraries/glew/GL/*)

		source_group("Engine" FILES ${ENGINE_SRC})
		source_group("Libraries\\adpcm" FILES ${ADPCM_SRC})
		source_group("Libraries\\huffman" FILES ${HUFFMAN_SRC})
		source_group("Libraries\\pkware" FILES ${PKWARE_SRC})
		source_group("Libraries\\SDL" FILES ${SDL_SRC})
		source_group("Libraries\\glew\\include" FILES ${GLEW_SRC})
		source_group("Libraries\\glm\\" FILES ${GLM_SRC})
		source_group("Shared" FILES ${SHARED_SRC})

		set(LIBRARY_SRC ${SDL_SRC} ${ADPCM_SRC} ${HUFFMAN_SRC} ${PKWARE_SRC} ${GLEW_SRC})

		set(ENGINE_SRC ${ENGINE_SRC} ${SHARED_SRC} ${LIBRARY_SRC})
		set(SDL2_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/sdl/x86")
		set(SDL2_LIBRARIES "${SDL2_LIBRARY_DIR}/SDL2.lib" "${SDL2_LIBRARY_DIR}/SDL2main.lib"
			"${SDL2_LIBRARY_DIR}/SDL2main.lib" "${SDL2_LIBRARY_DIR}/SDL2test.lib"
			"${SDL2_LIBRARY_DIR}/SDL2_net.lib" "${SDL2_LIBRARY_DIR}/SDL2_mixer.lib"
		)

		set(GLEW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glew")
		set(GLEW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glew/glew32.lib")
		set(GLM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glm")
	else()
		list(REMOVE_ITEM ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Platform_Windows.cpp)

		find_package(SDL2 REQUIRED)
		find_package(GLEW REQUIRED)
		find_package(GLM REQUIRED)
	endif()

	add_executable(game ${ENGINE_SRC})
	set_target_properties(game PROPERTIES OUTPUT_NAME game)
	set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)
	
	target_link_libraries(game PRIVATE ${SDL2_LIBRARIES} ${GLEW_LIBRARIES})
	target_include_directories(game PRIVATE ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

	if(WIN32)
		set_target_properties(game PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
	else()
		target_link_libraries(game PRIVATE m dl)
	endif()

	target_compile_definitions(game PUBLIC D2EXE)
endif()

# Build D2Common.dll
if(BUILD_D2CLIENT OR BUILD_D2SERVER)
	message("Including D2Common.dll files")

	file(GLOB_RECURSE D2COMMON_SRC Modcode/Common/*.h Modcode/Common/*hpp Modcode/Common/*.c Modcode/Common/*.cpp)

	source_group("Common" FILES ${D2COMMON_SRC})
	source_group("Shared" FILES ${SHARED_SRC})

	set(D2COMMON_SRC ${D2COMMON_SRC} ${SHARED_SRC})

	add_library(D2Common SHARED ${D2COMMON_SRC})
	set_target_properties(D2Common PROPERTIES OUTPUT_NAME D2Common)
	set_target_properties(D2Common PROPERTIES LINKER_LANGUAGE CXX)

	target_link_libraries(D2Common ${STATIC_LIBRARIES})
	target_compile_definitions(D2Common PUBLIC D2COMMON)

	link_directories(${PROJECT_BINARY_DIR})
endif()

# Build D2Client.dll
if(BUILD_D2CLIENT)
	message("Including D2Client.dll files")

	file(GLOB_RECURSE D2CLIENT_SRC Modcode/Client/*.h Modcode/Client/*.hpp Modcode/Client/*.c Modcode/Client/*.cpp)

	source_group("Client" FILES ${D2CLIENT_SRC})
	source_group("Shared" FILES ${SHARED_SRC})

	set(D2CLIENT_SRC ${D2CLIENT_SRC} ${SHARED_SRC})

	add_library(D2Client SHARED ${D2CLIENT_SRC})
	set_target_properties(D2Client PROPERTIES OUTPUT_NAME D2Client)
	set_target_properties(D2Client PROPERTIES LINKER_LANGUAGE CXX)

	target_link_libraries(D2Client ${STATIC_LIBRARIES} D2Common)
	target_compile_definitions(D2Client PUBLIC D2CLIENT)
endif()


# Build D2Server.dll
if(BUILD_D2SERVER)
	message("Including D2Server.dll files")

	file(GLOB_RECURSE D2SERVER_SRC Modcode/Server/*.h Modcode/Server/*.hpp Modcode/Server/*.c Modcode/Server/*.cpp)

	source_group("Server" FILES ${D2SERVER_SRC})
	source_group("Shared" FILES ${SHARED_SRC})

	set(D2SERVER_SRC ${D2SERVER_SRC} ${SHARED_SRC})

	add_library(D2Server SHARED ${D2SERVER_SRC})
	set_target_properties(D2Server PROPERTIES OUTPUT_NAME D2Server)
	set_target_properties(D2Server PROPERTIES LINKER_LANGUAGE CXX)

	target_link_libraries(D2Server ${STATIC_LIBRARIES} D2Common)
	target_compile_definitions(D2Server PUBLIC D2SERVER)
endif()
