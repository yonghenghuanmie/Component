cmake_minimum_required(VERSION 3.15)

project(StateMachine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(${PROJECT_NAME} StateMachine.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)

include(FetchContent)
FetchContent_Declare(
    Graph
    URL https://github.com/yonghenghuanmie/Component/archive/refs/tags/v1.5.2.tar.gz
    SOURCE_SUBDIR Graph
)
FetchContent_MakeAvailable(Graph)
target_link_libraries(${PROJECT_NAME} PRIVATE Graph)