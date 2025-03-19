# Grass-Rendering

## Screenshot
![03](https://github.com/user-attachments/assets/20b53d7b-861f-4508-bfd4-df9af61d5c22)

## Dependencies
* [GLFW](https://github.com/glfw/glfw) 
* [GLAD](https://glad.dav1d.de/) 
* [GLM](https://github.com/g-truc/glm) 
* [Assimp](https://github.com/assimp/assimp) 

## Build
### 1. Clone the Repository
```sh
git clone https://github.com/lanwenzhang/Software-Rasterizer.git
cd Software-Rasterizer
```

### 2. Install Required Dependencies
#### Windows
- Install [CMake](https://cmake.org/download/)
- Install a C++ compiler (MinGW or Visual Studio)

#### Linux/macOS
Ensure CMake and a C++ compiler (`g++`/`clang`) are installed:
```sh
sudo apt install cmake g++    # Ubuntu/Debian
brew install cmake            # macOS
```

### 3. Create a Build Directory
```sh
mkdir build
cd build
```

### 4. Run CMake to Configure the Project

#### Windows (Visual Studio Generator)
```sh
cmake .. -G "Visual Studio 17 2022"
```

#### Windows (MinGW)
```sh
cmake .. -G "MinGW Makefiles"
```

#### Linux/macOS
```sh
cmake ..
```

### 5. Compile the Project
#### Windows (Visual Studio)
Open `Software-Rasterizer.sln` in Visual Studio and build.

#### Windows (MinGW)
```sh
mingw32-make
```

#### Linux/macOS
```sh
make
```

### 6. Run the Executable
```sh
./SoftwareRasterizer  # Linux/macOS
SoftwareRasterizer.exe  # Windows
```

## References
* [Computer Graphics Programming in OpenGL with C++ 3rd Edition](https://www.packtpub.com/en-us/product/computer-graphics-programming-in-opengl-with-c-edition-3-9781836641186)
* [Learn OpenGL Tutorial](https://learnopengl.com)
