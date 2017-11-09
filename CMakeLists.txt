cmake_minimum_required(VERSION 3.0)

project(system)

#set(build_flags -DDEBUG)
#
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${build_flags}")

set(USE_C_FLAGS "-c -g -m32 -fno-builtin -Woverflow")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_C_FLAGS}")
add_subdirectory(boot)
add_subdirectory(kernel)
add_subdirectory(lib)


