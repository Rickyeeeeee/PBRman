# PBRman

Interactive CPU raytracer
![image](https://github.com/user-attachments/assets/a20ae827-09f4-4ddf-99ee-e1fa39ed34ea)

## Features

- CPU path tracing with four materials
  - Lambertian
  - Dielectirc (refraction and reflection)
  - Metal (with fuzziness)
  - Emissive
- Tile-based multi-threading
- BVH acceleration for meshes
- Interactive viewer using progressive rendering
- Interactive ui with nvrhi (DX12 only currently) and the ImGui library

## Requirements

- CMake >= 3.12
- C++ compiler (e.g., g++ or clang++)
- Visual Studio >= 2017

## Build Instructions

```bash
# Clone the repository
git clone --recursive https://github.com/Rickyeeeeee/PBRman.git
cd PBRman

# Create a build directory
mkdir build
cd build

# Run CMake and build
cmake ..

# Open visual studio
```

## Run
- Run with Visual Studio


