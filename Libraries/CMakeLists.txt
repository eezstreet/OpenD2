aux_source_directory(adpcm ADPCM_SRC)
add_library(adpcm ${ADPCM_SRC})
target_include_directories(adpcm INTERFACE .)

aux_source_directory(huffman HUFF_SRC)
add_library(huffman ${HUFF_SRC})
target_include_directories(huffman INTERFACE .)

aux_source_directory(pkware PKWARE_SRC)
add_library(pkware ${PKWARE_SRC})
target_include_directories(pkware INTERFACE .)
