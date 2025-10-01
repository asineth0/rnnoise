# RNNoise demo application
add_executable(rnnoise_demo rnnoise_demo.c)

# Link against the rnnoise library
target_link_libraries(rnnoise_demo PRIVATE rnnoise)

# Set include directories
target_include_directories(rnnoise_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Install the demo (optional)
install(TARGETS rnnoise_demo
    RUNTIME DESTINATION bin
)