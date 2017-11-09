cmake_minimum_required(VERSION 3.0)

project(system)

#set(build_flags -DDEBUG)
#
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${build_flags}")

add_subdirectory(boot)
add_subdirectory(kernel)
add_subdirectory(lib)


