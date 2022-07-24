cmake_minimum_required(VERSION 3.15)

project(Graph)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
install(FILES Graph.h DESTINATION include)

project(Example)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(${PROJECT_NAME} Example.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} PRIVATE Graph)

project(Performance)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(${PROJECT_NAME} Performance.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} PRIVATE Graph)

include(FetchContent)
FetchContent_Declare(
    Elapse
    URL https://github.com/yonghenghuanmie/Component/archive/refs/tags/v1.5.6.tar.gz
    SOURCE_SUBDIR Elapse
)
FetchContent_MakeAvailable(Elapse)
target_link_libraries(${PROJECT_NAME} PRIVATE Elapse)