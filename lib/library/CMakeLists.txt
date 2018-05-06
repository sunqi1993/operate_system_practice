include_directories(
        .
        ../
        ../../kernel
)

file(GLOB src
        *.c
        )

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build_lib)

add_library(library ${src})
