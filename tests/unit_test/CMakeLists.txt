cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest REQUIRED)

add_executable(unit_tests tree_test.cpp)

target_link_libraries(unit_tests
									GTest::GTest
					  				GTest::Main)

target_include_directories(unit_tests PUBLIC ${PROJECT_SOURCE_DIR}/include)

enable_testing()
add_test(NAME Tests COMMAND tests)
