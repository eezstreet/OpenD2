cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0015 NEW)

project(OpenD2)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

option(BUILD_GAME "Build Executable" ON)
option(BUILD_D2CLIENT "Build D2Client" ON)
option(BUILD_D2GAME "Build D2Game" ON)

# Common options
set(STATIC_LIBRARIES dbghelp.lib psapi.lib)

# Add shared files
file(GLOB_RECURSE SHARED_SRC Shared/*.h Shared/*.hpp Shared/*.c Shared/*.cpp)
source_group("Shared" FILES ${SHARED_SRC})

# Build game.exe
if(BUILD_GAME)
	message("Including game.exe files")

	file(GLOB_RECURSE GAME_SRC Game/*.h Game/*.hpp Game/*.c Game/*.cpp)

	file(GLOB_RECURSE SDL_SRC Libraries/sdl/*)
	file(GLOB_RECURSE ADPCM_SRC Libraries/adpcm/*)
	file(GLOB_RECURSE HUFFMAN_SRC Libraries/huffman/*)
	file(GLOB_RECURSE PKWARE_SRC Libraries/pkware/*)

	source_group("Game" FILES ${GAME_SRC})
	source_group("Libraries\\adpcm" FILES ${ADPCM_SRC})
	source_group("Libraries\\huffman" FILES ${HUFFMAN_SRC})
	source_group("Libraries\\pkware" FILES ${PKWARE_SRC})
	source_group("Libraries\\SDL" FILES ${SDL_SRC})
	source_group("Shared" FILES ${SHARED_SRC})

	set(LIBRARY_SRC ${SDL_SRC} ${ADPCM_SRC} ${HUFFMAN_SRC} ${PKWARE_SRC})

	set(GAME_SRC ${GAME_SRC} ${SHARED_SRC} ${LIBRARY_SRC})

	link_directories("Shared")
	link_directories("Libraries/sdl/x86")

	add_executable(game WIN32 ${GAME_SRC})
	set_target_properties(game PROPERTIES OUTPUT_NAME game)
	set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

	set(SDL_LIBRARY SDL2.lib SDL2main.lib SDL2test.lib SDL2_net.lib)

	target_link_libraries(game ${STATIC_LIBRARIES} ${SDL_LIBRARY})
	target_compile_definitions(game PUBLIC D2EXE)
endif()

# Build D2Common.dll
if(BUILD_D2CLIENT OR BUILD_D2GAME)
	message("Including D2Common.dll files")

	file(GLOB_RECURSE D2COMMON_SRC Common/*.h Common/*hpp Common/*.c Common/*.cpp)

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

	file(GLOB_RECURSE D2CLIENT_SRC Client/*.h Client/*.hpp Client/*.c Client/*.cpp)

	source_group("Client" FILES ${D2CLIENT_SRC})
	source_group("Shared" FILES ${SHARED_SRC})

	set(D2CLIENT_SRC ${D2CLIENT_SRC} ${SHARED_SRC})

	add_library(D2Client SHARED ${D2CLIENT_SRC})
	set_target_properties(D2Client PROPERTIES OUTPUT_NAME D2Client)
	set_target_properties(D2Client PROPERTIES LINKER_LANGUAGE CXX)

	target_link_libraries(D2Client ${STATIC_LIBRARIES})
	target_compile_definitions(D2Client PUBLIC D2CLIENT)
endif()


# Build D2Game.dll
#if(BUILD_D2GAME)
#	message("Including D2Game.dll files")
#
#	file(GLOB_RECURSE D2GAME_SRC Server/*.h Server/*.hpp Server/*.c Server/*.cpp)
#	source_group("Server" FILES ${D2GAME_SRC})
#	set(D2GAME_SRC ${D2GAME_SRC} ${SHARED_SRC})
#
#	add_library(D2Game SHARED ${D2GAME_SRC})
#	set_target_properties(D2Game PROPERTIES OUTPUT_NAME D2Game)
#	set_target_properties(D2Game PROPERTIES LINKER_LANGUAGE CXX)
#	target_link_libraries(D2Game ${STATIC_LIBRARIES})
#	target_compile_definitions(D2Game PUBLIC D2GAME)
#endif