include_directories(
        .
        ../io
)

file(GLOB src
        *.S
        )

#只编译不连接
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm S)


#只是生成汇编的预处理文件
set(CMAKE_ASM_NASM_FLAGS "-f elf")


ENABLE_LANGUAGE(ASM_NASM)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build_lib)

add_library(io STATIC ${src})