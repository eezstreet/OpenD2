project(game)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} GAME_SOURCES)

if(WIN32)
    list(REMOVE_ITEM GAME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Platform_Linux.cpp)
    set(SDL2_LIBS SDL2::Core SDL2::Mixer SDL2::Net)
    set(EXETYPE "WIN32")
    set(WINDEFS -DWIN32_LEAN_AND_MEAN)
else()
    list(REMOVE_ITEM GAME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Platform_Windows.cpp)
    set(SDL2_LIBS ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_NET_LIBRARIES})
endif()


add_executable(${PROJECT_NAME} ${EXETYPE} ${GAME_SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE -DD2EXE ${WINDEFS})
target_link_libraries(${PROJECT_NAME} shared GLUT::GLUT GLEW::Static ${SDL2_LIBS} adpcm huffman pkware)

add_dependencies(${PROJECT_NAME} D2Client D2Server)
