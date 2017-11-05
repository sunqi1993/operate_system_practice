include_directories(
        .
        ../../kernel
)

file(GLOB src
        *.c
        )
set(USE_C_FLAGS "-c -g -m32 -fno-builtin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_C_FLAGS}")

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build_lib)

add_library(library ${src})
