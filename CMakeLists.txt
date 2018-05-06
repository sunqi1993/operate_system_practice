cmake_minimum_required(VERSION 3.0)

project(system)

#set(build_flags -DDEBUG)
#
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${build_flags}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions( -DSchedule)

set(USE_C_FLAGS "-c -g -m32 -fno-builtin -Woverflow -Wincompatible-pointer-types")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_C_FLAGS}")

set(USE_CXX_FLAGS "-fpermissive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_CXX_FLAGS} ${USE_C_FLAGS}")

add_subdirectory(boot)
add_subdirectory(kernel)
add_subdirectory(lib)


