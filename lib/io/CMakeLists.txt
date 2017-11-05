include_directories(
        .
        ../
)

file(GLOB src
        ./*.S
        )

#只编译不连接
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm S)


#只是生成汇编的预处理文件  这个一定要提前设置因为不设置的话默认是以平台参数设置输出格式
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)


ENABLE_LANGUAGE(ASM_NASM)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build_lib)
#set(CMAKE_ASM_NASM_LINK_EXECUTABLE "cp <OBJECTS>  <TARGET> ")

#set(CMAKE_LINK_LIBRARY_FLAG "-f elf")

add_library(io  ${src})

