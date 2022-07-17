cmake_minimum_required(VERSION 3.15)

project(Graph)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
install(FILES Graph.h DESTINATION include)