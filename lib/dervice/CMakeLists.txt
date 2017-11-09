include_directories(
        ../io
)

#只编译不连接


set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build_lib)

add_library(timer STATIC timer.c)