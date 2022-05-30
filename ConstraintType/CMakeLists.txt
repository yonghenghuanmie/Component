cmake_minimum_required(VERSION 3.15)
set(PROJECT_VERSION 1.2.0)

function(CompilerOption)
    # Show actual C++ version for MSVC
    # Use compliant C+11 preprocessor for MSVC
    if(MSVC)
        add_compile_options(/Zc:__cplusplus /Zc:preprocessor)
        add_compile_definitions(ML99_ALLOW_POOR_DIAGNOSTICS)
    # Metalang99 relies on heavy macro machinery. To avoid useless macro expansion
    # errors, please write this:
    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
      target_compile_options(${PROJECT_NAME} PRIVATE -fmacro-backtrace-limit=1)
    elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
      target_compile_options(${PROJECT_NAME} PRIVATE -ftrack-macro-expansion=0)
    endif()
endfunction()

function(Dependency)
    # Dependency
    include(FetchContent)
    FetchContent_Declare(
        metalang99
        URL https://github.com/Hirrolot/metalang99/archive/refs/tags/v1.13.1.tar.gz # v1.13.1
    )
    FetchContent_MakeAvailable(metalang99)
endfunction()

function(Example standard file_name)
    CompilerOption()
    Dependency()
    set(CMAKE_CXX_STANDARD ${standard})
    set(CMAKE_CXX_STANDARD_REQUIRED)
    add_executable(${PROJECT_NAME} ${file_name})
    target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
    target_link_libraries(${PROJECT_NAME} PRIVATE metalang99)
endfunction()

# Example for C++ 14
project(ConstraintTypeExample14 VERSION ${PROJECT_VERSION})
    Example(14 ConstraintTypeBackwardCompatibility.cpp)

# Example for C++ 17
project(ConstraintTypeExample17 VERSION ${PROJECT_VERSION})
    Example(17 ConstraintTypeBackwardCompatibility.cpp)

# Example for C++ 20
project(ConstraintTypeExample20 VERSION ${PROJECT_VERSION})
    Example(20 ConstraintType.cpp)

# Header library
CompilerOption()
Dependency()
project(ConstraintType VERSION ${PROJECT_VERSION})
    add_library(${PROJECT_NAME} INTERFACE)
    target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
    install(FILES ConstraintType.h DESTINATION include)