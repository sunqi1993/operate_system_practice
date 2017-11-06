#头文件包含的目录 使用nasm的时候 由于目录不以 dir/ 结束所以无效 yasm是nasm的兼容版 没有这个bug
include_directories(
        .
        ../lib/dervice
        ../lib/io
        ../lib/library
)
ENABLE_LANGUAGE(ASM_NASM)
file(GLOB src
        *.c
        *.S
        )

if (WIN32)
    set(CMAKE_ASM_NASM_COMPILER "D:/MySoftWare/chocolatey_package/bin")
endif ()

set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm S)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)

#修改输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#add_executable(loader.bin loader.s)
#set_target_properties(loader.bin PROPERTIES LINKER_LANGUAGE NASM)
#add_executable(mbr.bin mbr.s)
#set_target_properties(mbr.bin PROPERTIES LINKER_LANGUAGE NASM)

set(USE_C_FLAGS "-c -g -m32 -fno-builtin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_C_FLAGS}")

set(CMAKE_C_LINK_EXECUTABLE  "ld -Ttext 0xc0001500 -e main -O <TARGET> -o <TARGET> <LINK_LIBRARIES>")


add_executable(kernel ${src})

target_link_libraries(kernel io timer library)

set_target_properties(kernel
        PROPERTIES
        LINK_LANGUAGE C
        SUFFIX .bin
        )
