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
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES>  -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} <FLAGS> -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm S)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_ASM_NASM_FLAGS "-f elf")



enable_language(ASM_NASM)

#修改输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


set(USE_C_FLAGS "-c -g -m32 -fno-builtin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_C_FLAGS}")

set(KERNEL_LINK_FLAGS  "-Ttext 0xc0001500 -melf_i386 -e main -Map=${PROJECT_SOURCE_DIR}/bin/kernel.map")

set(CMAKE_C_LINK_EXECUTABLE  "ld ${KERNEL_LINK_FLAGS}  -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")



add_executable(kernel ${src})

target_link_libraries(kernel io timer library)

set_target_properties(kernel
        PROPERTIES
        LINK_LANGUAGE C
        SUFFIX .bin
        )

message(STATUS "${CMAKE_ASM_NASM_OBJECT_FORMAT}")