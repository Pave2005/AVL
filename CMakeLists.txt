cmake_minimum_required(VERSION 3.14)

project(tree)

enable_testing()
add_subdirectory(unit_tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(PIC "Builds a tree graph" OFF)

if(PIC)
	add_definitions(-DPIC)
endif()

set(RELEASE_COMPILE_OPTIONS
	-O2
	-Wformat-security
	-Wformat=2
	-Wall
	-Wextra
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wcast-align
	-Wpointer-arith
	-Wunreachable-code
)

add_executable(tree ./tree.cpp)
target_include_directories(tree PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/get_info)

target_compile_options(tree PRIVATE ${RELEASE_COMPILE_OPTIONS})
