

aux_source_directory(Common D2COMMON_SRC)
add_library(D2Common SHARED ${D2COMMON_SRC})
target_link_libraries(D2Common shared)

aux_source_directory(Client D2CLIENT_SRC)
add_library(D2Client SHARED ${D2CLIENT_SRC})
target_link_libraries(D2Client D2Common)

aux_source_directory(Server D2SERVER_SRC)
add_library(D2Server SHARED ${D2SERVER_SRC})
target_link_libraries(D2Server D2Common)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
