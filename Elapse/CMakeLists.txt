cmake_minimum_required(VERSION 3.15)

project(Elapse)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
install(FILES Elapse.h DESTINATION include)
