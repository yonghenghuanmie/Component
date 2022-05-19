cmake_minimum_required(VERSION 3.15)
set(PROJECT_VERSION 1.1.0)


project(ConstraintTypeExample14 VERSION ${PROJECT_VERSION})
project(ConstraintTypeExample17 VERSION ${PROJECT_VERSION})
project(ConstraintTypeExample20 VERSION ${PROJECT_VERSION})

# Show actual C++ version for MSVC
# Use compliant C+11 preprocessor for MSVC
if(MSVC)
    add_compile_options(/Zc:__cplusplus /Zc:preprocessor)
    add_compile_definitions(ML99_ALLOW_POOR_DIAGNOSTICS)
endif()

# Example for C++ 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED)
add_executable(ConstraintTypeExample14 ConstraintTypeBackwardCompatibility.cpp)
target_include_directories(ConstraintTypeExample14 PUBLIC $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)
unset(CMAKE_CXX_STANDARD)
unset(CMAKE_CXX_STANDARD_REQUIRED)

# Example for C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
add_executable(ConstraintTypeExample17 ConstraintTypeBackwardCompatibility.cpp)
target_include_directories(ConstraintTypeExample17 PUBLIC $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)
unset(CMAKE_CXX_STANDARD)
unset(CMAKE_CXX_STANDARD_REQUIRED)

# Example for C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
add_executable(ConstraintTypeExample20 ConstraintType.cpp)
target_include_directories(ConstraintTypeExample20 PUBLIC $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)
unset(CMAKE_CXX_STANDARD)
unset(CMAKE_CXX_STANDARD_REQUIRED)

# Header library
project(ConstraintType VERSION ${PROJECT_VERSION})
add_library(ConstraintType INTERFACE)
target_include_directories(ConstraintType INTERFACE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>)
install(FILES ConstraintType.h DESTINATION include)

# Dependency
include(FetchContent)
FetchContent_Declare(
    metalang99
    URL https://github.com/Hirrolot/metalang99/archive/refs/tags/v1.13.1.tar.gz # v1.13.1
)
FetchContent_MakeAvailable(metalang99)
# Metalang99 relies on heavy macro machinery. To avoid useless macro expansion
# errors, please write this:
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  target_compile_options(ConstraintTypeExample14 PRIVATE -fmacro-backtrace-limit=1)
  target_compile_options(ConstraintTypeExample17 PRIVATE -fmacro-backtrace-limit=1)
  target_compile_options(ConstraintTypeExample20 PRIVATE -fmacro-backtrace-limit=1)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(ConstraintTypeExample14 PRIVATE -ftrack-macro-expansion=0)
  target_compile_options(ConstraintTypeExample17 PRIVATE -ftrack-macro-expansion=0)
  target_compile_options(ConstraintTypeExample20 PRIVATE -ftrack-macro-expansion=0)
endif()

# Link for all the example code and header library
target_link_libraries(ConstraintType INTERFACE metalang99)
target_link_libraries(ConstraintTypeExample14 PUBLIC metalang99)
target_link_libraries(ConstraintTypeExample17 PUBLIC metalang99)
target_link_libraries(ConstraintTypeExample20 PUBLIC metalang99)