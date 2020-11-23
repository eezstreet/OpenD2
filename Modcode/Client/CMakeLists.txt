file(GLOB_RECURSE D2CLIENT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

source_group("UI/Menus" "${CMAKE_CURRENT_SOURCE_DIR}/UI/Menus")
source_group("UI/Panels" "${CMAKE_CURRENT_SOURCE_DIR}/UI/Panels")
source_group("UI/Widgets" "${CMAKE_CURRENT_SOURCE_DIR}/UI/Widgets")
source_group("UI" "${CMAKE_CURRENT_SOURCE_DIR}/UI")
source_group("Shared" "${SHARED_SRC}")

add_library(D2Client SHARED
    "${D2CLIENT_FILES}"
    "${SHARED_SRC}"
)
set_target_properties(D2Client PROPERTIES OUTPUT_NAME D2Client)
set_target_properties(D2Client PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(D2Client ${STATIC_LIBRARIES} D2Common)
target_compile_definitions(D2Client PUBLIC D2CLIENT)
