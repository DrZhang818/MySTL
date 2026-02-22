cmake_minimum_required(VERSION 3.10)
project(MySTL_Handmade LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")

enable_testing()

foreach(src ${TEST_SOURCES})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src})
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
  add_test(NAME ${name} COMMAND ${name})
endforeach()
