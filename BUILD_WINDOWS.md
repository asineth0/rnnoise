# Building RNNoise on Windows with CMake and MSVC

This document provides instructions for building RNNoise on Windows using CMake and Microsoft Visual C++ (MSVC).

## Prerequisites

- **Visual Studio 2019 or later** (with C++ development tools)
- **CMake 3.10 or later**
- **Python 3.x** (for generating model data, if needed)
- **Git** (for cloning the repository)

## Quick Start

1. **Clone the repository:**
   ```cmd
   git clone https://github.com/asineth0/rnnoise.git
   cd rnnoise
   ```

2. **Download the default model (if not already present):**
   ```cmd
   # The model files should already be included in the src/ directory
   # If missing, you can download them manually:
   powershell -Command "Invoke-WebRequest -Uri 'https://media.xiph.org/rnnoise/models/rnnoise_data-0a8755f8e2d834eff6a54714ecc7d75f9932e845df35f8b59bc52a7cfe6e8b37.tar.gz' -OutFile 'rnnoise_data.tar.gz'"
   tar -xzf rnnoise_data.tar.gz
   ```

3. **Create build directory and configure:**
   ```cmd
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" -A x64
   ```

4. **Build the project:**
   ```cmd
   cmake --build . --config Release
   ```

## Build Outputs

After a successful build, you'll find the following files in the `build` directory:

### Libraries and Executables
- `src/Release/rnnoise.dll` - Main RNNoise dynamic library
- `src/Release/rnnoise.lib` - Import library for linking
- `examples/Release/rnnoise_demo.exe` - Demo application
- `src/Release/dump_features.exe` - Feature extraction utility
- `src/Release/dump_weights_blob.exe` - Model weights export utility

### Generated Files
- `build/config.h` - Configuration header
- `build/rnnoise.pc` - pkg-config file
- `weights_blob.bin` - Exported model weights (created by dump_weights_blob)

## Using the Demo Application

The `rnnoise_demo.exe` processes raw 16-bit PCM audio files:

```cmd
# Copy the DLL to the same directory as the demo (required for execution)
copy src\Release\rnnoise.dll examples\Release\

# Run the demo
examples\Release\rnnoise_demo.exe input.raw output.raw
```

**Input format:** Raw 16-bit signed PCM, 48 kHz sample rate, mono
**Frame size:** 480 samples (10ms at 48 kHz)

## Build Configuration Options

### Debug Build
```cmd
cmake --build . --config Debug
```

### Custom Generator
For different Visual Studio versions:
```cmd
# Visual Studio 2022
cmake .. -G "Visual Studio 17 2022" -A x64

# Visual Studio 2019
cmake .. -G "Visual Studio 16 2019" -A x64
```

### Build Specific Targets
```cmd
# Build only the library
cmake --build . --config Release --target rnnoise

# Build only the demo
cmake --build . --config Release --target rnnoise_demo
```

## Utilities

### dump_features.exe
Extracts features from audio for training:
```cmd
src\Release\dump_features.exe [-rir_list list] <speech> <noise> <fg_noise> <output> <count>
```

### dump_weights_blob.exe
Exports model weights to a binary file:
```cmd
src\Release\dump_weights_blob.exe
# Creates weights_blob.bin in the current directory
```

## Integration in Your Project

### Using CMake
```cmake
find_package(rnnoise REQUIRED)
target_link_libraries(your_target rnnoise::rnnoise)
```

### Manual Integration
1. Copy `rnnoise.dll` and `rnnoise.lib` to your project
2. Include `include/rnnoise.h` in your source
3. Link against `rnnoise.lib`

### Basic Usage Example
```c
#include "rnnoise.h"

#define FRAME_SIZE 480

int main() {
    DenoiseState *st = rnnoise_create(NULL);
    float frame[FRAME_SIZE];
    
    // Process audio frame
    rnnoise_process_frame(st, frame, frame);
    
    rnnoise_destroy(st);
    return 0;
}
```

## Troubleshooting

### Common Issues

1. **Missing DLL Error:**
   - Ensure `rnnoise.dll` is in the same directory as your executable
   - Or add the DLL location to your system PATH

2. **Build Errors:**
   - Verify Visual Studio C++ tools are installed
   - Check CMake version (minimum 3.10 required)
   - Ensure you're using a supported Visual Studio version

3. **Model Data Missing:**
   - The repository includes default model files in `src/`
   - If missing, download using the instructions above

### Build Dependencies

The build system automatically handles:
- Windows-specific compatibility fixes
- Conditional compilation for different platforms
- Model data integration
- Library export configuration

## Performance Notes

- The Release build is significantly faster than Debug
- The library uses SIMD optimizations when available
- Frame processing is optimized for real-time audio (10ms frames)

## License

RNNoise is licensed under the BSD 3-Clause License. See the COPYING file for details.