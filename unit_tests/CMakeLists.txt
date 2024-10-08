cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../")
add_definitions(-DTEST_DATA_DIR=\"${TEST_DATA_DIR}\")

find_package(GTest REQUIRED)

set(COMPILE_OPTIONS
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

add_executable(tests
			   tests.cpp
			   main.cpp)

target_compile_options(tests PRIVATE ${COMPILE_OPTIONS})

target_link_libraries(tests
					  GTest::GTest
					  GTest::Main)

target_include_directories(tests PRIVATE
	                       ${CMAKE_SOURCE_DIR}/get_info
	                       ${CMAKE_SOURCE_DIR}/include)

enable_testing()
add_test(NAME Tests COMMAND tests)
