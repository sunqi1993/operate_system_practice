
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm S)
set(CMAKE_ASM_NASM_OBJECT_FORMAT bin)
#set(CMAKE_NASM_LINK_EXECUTABLE nasm)
if (WIN32)
    set(CMAKE_ASM_NASM_COMPILER "D:/MySoftWare/chocolatey_package/bin/")
endif ()

set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f bin -l${PROJECT_SOURCE_DIR}/bin/loader.map")
#只是生成汇编的预处理文件

#set(CMAKE_ASM_NASM_LINK_EXECUTABLE "ld <FLAGS> <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")
#这个地方不需要连接的过程 原本的程序是调用了两次nasm 会报错
#set(CMAKE_ASM_NASM_LINK_EXECUTABLE "cp <OBJECTS>  <TARGET>")
#set(CMAKE_ASM_NASM_LINK_EXECUTABLE "yasm <FLAGS> -o <TARGET> <OBJECTS> ")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "cp <OBJECTS>  <TARGET> ")
ENABLE_LANGUAGE(ASM_NASM)


#头文件包含的目录 使用nasm的时候 由于目录不以 dir/ 结束所以无效 yasm是nasm的兼容版 没有这个bug
include_directories(
        include
)
#nasm修改方式
#add_compile_options(-I ${CMAKE_CURRENT_SOURCE_DIR}/include/ )


#修改输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#add_executable(loader.bin loader.s)
#set_target_properties(loader.bin PROPERTIES LINKER_LANGUAGE NASM)
#add_executable(mbr.bin mbr.s)
#set_target_properties(mbr.bin PROPERTIES LINKER_LANGUAGE NASM)

add_executable(loader loader.S)
add_executable(mbr mbr.S)
#add_library(1  STATIC loader.S)

set_target_properties(mbr loader
        PROPERTIES

        SUFFIX .bin
        )