project(shared)

aux_source_directory(. SHARED_SRC)
add_library(${PROJECT_NAME} ${SHARED_SRC})
