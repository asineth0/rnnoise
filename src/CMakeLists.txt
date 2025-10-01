# Core RNNoise library sources
set(RNNOISE_SOURCES
    denoise.c
    rnn.c
    pitch.c
    kiss_fft.c
    celt_lpc.c
    nnet.c
    nnet_default.c
    parse_lpcnet_weights.c
    rnnoise_data.c
    rnnoise_tables.c
)

# Platform-specific optimized sources
set(RNNOISE_SOURCES_SSE4_1
    x86/nnet_sse4_1.c
)

set(RNNOISE_SOURCES_AVX2
    x86/nnet_avx2.c
)

set(X86_RTCD_SOURCES
    x86/x86_dnn_map.c
    x86/x86cpu.c
)

# Create the main library target
add_library(rnnoise ${RNNOISE_SOURCES})

# Set library properties
set_target_properties(rnnoise PROPERTIES
    VERSION 0.4.1
    SOVERSION 0
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/../include/rnnoise.h"
)

# Add compile definitions
target_compile_definitions(rnnoise PRIVATE RNNOISE_BUILD)

if(RNNOISE_ENABLE_ASSERTIONS)
    target_compile_definitions(rnnoise PRIVATE RNNOISE_ENABLE_ASSERTIONS)
endif()

if(NOT RNNOISE_DNN_DEBUG_FLOAT)
    target_compile_definitions(rnnoise PRIVATE DISABLE_DEBUG_FLOAT)
endif()

# Handle x86 optimizations
if(RNNOISE_ENABLE_X86_RTCD)
    target_sources(rnnoise PRIVATE ${X86_RTCD_SOURCES} ${RNNOISE_SOURCES_SSE4_1} ${RNNOISE_SOURCES_AVX2})
    target_compile_definitions(rnnoise PRIVATE RNN_ENABLE_X86_RTCD CPU_INFO_BY_ASM)
    
    # Set compiler flags for optimized sources
    if(NOT MSVC)
        set_source_files_properties(${RNNOISE_SOURCES_SSE4_1} PROPERTIES
            COMPILE_FLAGS "-msse4.1"
        )
        set_source_files_properties(${RNNOISE_SOURCES_AVX2} PROPERTIES
            COMPILE_FLAGS "-mavx -mfma -mavx2"
        )
    else()
        # MSVC uses different flags
        set_source_files_properties(${RNNOISE_SOURCES_SSE4_1} PROPERTIES
            COMPILE_FLAGS "/arch:SSE2"  # MSVC doesn't have separate SSE4.1 flag
        )
        set_source_files_properties(${RNNOISE_SOURCES_AVX2} PROPERTIES
            COMPILE_FLAGS "/arch:AVX2"
        )
    endif()
endif()

# Link libraries
target_link_libraries(rnnoise PRIVATE ${MATH_LIBS} ${LRINTF_LIBS})

# Set include directories
target_include_directories(rnnoise
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/../
)

# Windows-specific settings
if(WIN32)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(rnnoise PRIVATE DLL_EXPORT)
        if(MSVC)
            # Export all symbols for MSVC
            set_target_properties(rnnoise PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
        endif()
    endif()
endif()

# Install the library
install(TARGETS rnnoise
    EXPORT rnnoiseTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

# Create utility programs
add_executable(dump_features dump_features.c denoise.c pitch.c celt_lpc.c kiss_fft.c parse_lpcnet_weights.c rnnoise_tables.c)
target_compile_definitions(dump_features PRIVATE TRAINING)
target_link_libraries(dump_features PRIVATE ${MATH_LIBS})
target_include_directories(dump_features PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../)

add_executable(dump_weights_blob write_weights.c rnnoise_data.c)
target_compile_definitions(dump_weights_blob PRIVATE DUMP_BINARY_WEIGHTS)
target_link_libraries(dump_weights_blob PRIVATE ${MATH_LIBS})
target_include_directories(dump_weights_blob PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../)

# Export targets for find_package
install(EXPORT rnnoiseTargets
    FILE rnnoiseTargets.cmake
    NAMESPACE rnnoise::
    DESTINATION lib/cmake/rnnoise
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    rnnoiseConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/rnnoiseConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/rnnoiseConfig.cmake"
    INSTALL_DESTINATION lib/cmake/rnnoise
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/rnnoiseConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/rnnoiseConfigVersion.cmake"
    DESTINATION lib/cmake/rnnoise
)