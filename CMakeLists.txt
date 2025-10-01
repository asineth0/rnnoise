cmake_minimum_required(VERSION 3.10)

# Read version from package_version file or set default
set(RNNOISE_VERSION "unknown")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/package_version")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/package_version" VERSION_FILE_CONTENT)
    string(REGEX MATCH "PACKAGE_VERSION=\"([^\"]+)\"" VERSION_MATCH "${VERSION_FILE_CONTENT}")
    if(VERSION_MATCH)
        set(RNNOISE_VERSION "${CMAKE_MATCH_1}")
    endif()
endif()

project(rnnoise VERSION 0.4.1 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build options
option(RNNOISE_BUILD_EXAMPLES "Build example applications" ON)
option(RNNOISE_BUILD_SHARED_LIBS "Build shared libraries" ON)
option(RNNOISE_ENABLE_ASSERTIONS "Enable assertions in code" OFF)
option(RNNOISE_ENABLE_X86_RTCD "Enable x86 runtime CPU detection" OFF)
option(RNNOISE_DNN_DEBUG_FLOAT "Use floating-point DNN computation everywhere" OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Configure shared/static library build
if(RNNOISE_BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
else()
    set(BUILD_SHARED_LIBS OFF)
endif()

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    add_definitions(-D_WIN32_WINNT=0x0501)
    if(MSVC)
        # MSVC-specific settings
        add_compile_options(/W3)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        # Disable specific MSVC warnings that are common in C code
        add_compile_options(/wd4244 /wd4267 /wd4305)
    else()
        # MinGW settings
        add_definitions(-U__STRICT_ANSI__)
    endif()
else()
    # Unix-like systems
    add_compile_options(-Wall -Wextra -Wno-sign-compare -Wno-parentheses -Wno-long-long)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fvisibility=hidden)
    endif()
endif()

# Find required libraries
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    set(MATH_LIBS ${MATH_LIBRARY})
endif()

# Check for lrintf
include(CheckSymbolExists)
check_symbol_exists(lrintf "math.h" HAVE_LRINTF)
if(NOT HAVE_LRINTF)
    find_library(LRINTF_LIBRARY m)
    if(LRINTF_LIBRARY)
        set(LRINTF_LIBS ${LRINTF_LIBRARY})
    endif()
endif()

# Configure header
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Add include directories
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Add subdirectories
add_subdirectory(src)

if(RNNOISE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install headers
install(FILES include/rnnoise.h
        DESTINATION include)

# Generate and install pkg-config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/rnnoise.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/rnnoise.pc"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rnnoise.pc"
        DESTINATION lib/pkgconfig)

# Print configuration summary
message(STATUS "")
message(STATUS "RNNoise ${RNNOISE_VERSION} Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared libraries: ${RNNOISE_BUILD_SHARED_LIBS}")
message(STATUS "  Examples: ${RNNOISE_BUILD_EXAMPLES}")
message(STATUS "  Assertions: ${RNNOISE_ENABLE_ASSERTIONS}")
message(STATUS "  x86 RTCD: ${RNNOISE_ENABLE_X86_RTCD}")
message(STATUS "  DNN debug float: ${RNNOISE_DNN_DEBUG_FLOAT}")
message(STATUS "")