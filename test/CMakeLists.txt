cmake_minimum_required(VERSION 3.10)
project(SampleTest)

enable_testing()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(sample_test sample_test.cpp)
target_link_libraries(sample_test gtest gtest_main)

add_test(NAME SampleTest COMMAND sample_test)