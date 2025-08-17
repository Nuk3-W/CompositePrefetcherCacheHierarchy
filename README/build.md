## Build tutorial by platform and compiler

All builds use CMake. Create out-of-source build directories to keep your tree clean.

Common steps
- Ensure CMake is installed.
- Choose a generator (Makefiles, Ninja, Visual Studio, Xcode).
- Use separate build dirs per configuration or compiler.

Notes on configurations
- Single-config generators (Unix Makefiles, Ninja) use `-DCMAKE_BUILD_TYPE=Release` (or Debug).
- Multi-config generators (Visual Studio, Xcode) pass `--config Release` (or Debug) at build time.

### Windows (MSVC, Visual Studio)

Visual Studio 2022 (recommended)
1. Open a "x64 Native Tools Command Prompt for VS 2022".
2. Generate project files:
   - `cmake -S . -B build-msvc -G "Visual Studio 17 2022" -A x64`
3. Build (Release example):
   - `cmake --build build-msvc --config Release`
4. Run from the build directory:
   - `cd build-msvc`
   - `Release\CacheSim.exe`

MSVC + Ninja
1. Open the same VS developer prompt.
2. Generate:
   - `cmake -S . -B build-ninja -G Ninja -DCMAKE_BUILD_TYPE=Release`
3. Build:
   - `cmake --build build-ninja`
4. Run:
   - `cd build-ninja`
   - `CacheSim.exe`

MinGW-w64 (GCC)
1. Open a MinGW-w64 shell (ensure `gcc`/`g++` first on PATH).
2. Generate:
   - `cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release`
3. Build:
   - `cmake --build build-mingw`
4. Run:
   - `cd build-mingw`
   - `CacheSim.exe`

### Linux (GCC)
1. Install tools (Ubuntu/Debian example):
   - `sudo apt update`
   - `sudo apt install -y build-essential cmake`
2. Generate:
   - `cmake -S . -B build-gcc -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release`
3. Build:
   - `cmake --build build-gcc -j`
4. Run:
   - `cd build-gcc`
   - `./CacheSim`

### Linux (Clang)
1. Install tools:
   - `sudo apt update`
   - `sudo apt install -y clang cmake`
2. Generate:
   - `cmake -S . -B build-clang -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++`
3. Build:
   - `cmake --build build-clang -j`
4. Run:
   - `cd build-clang`
   - `./CacheSim`

### macOS (Apple Clang)
1. Install Command Line Tools (or Xcode) and CMake (Homebrew: `brew install cmake`).
2. Makefiles generator:
   - `cmake -S . -B build-macos -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release`
   - `cmake --build build-macos -j`
   - `cd build-macos`
   - `./CacheSim`
3. Xcode generator (optional):
   - `cmake -S . -B build-xcode -G Xcode`
   - `cmake --build build-xcode --config Release`
   - `cd build-xcode`
   - `Release/CacheSim`

### Changing configuration
- Single-config generators: re-generate with a new `-DCMAKE_BUILD_TYPE` into a separate directory (e.g., `build-debug`).
- Multi-config generators: keep one build tree and switch with `--config Debug|Release`.

### Cleaning builds
- Remove the build directory (e.g., `rm -rf build-gcc`) and re-run the generate step.

### Troubleshooting
- "No CMAKE_CXX_COMPILER could be found": install a compiler toolchain and ensure it’s on PATH.
- Wrong generator: specify `-G` explicitly as shown above.
- Architecture mismatch on Windows: add `-A x64` for Visual Studio.


