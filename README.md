# 🛠VisualEngine

![VisualEngine Editor](https://raw.githubusercontent.com/VisualBridge/VisualEngine/refs/heads/main/assets/Banner.png)

**VisualEngine** is a node-based 3D engine and level editor designed to be lean, fast, and powerful. 

The project includes the engine core and the **Wrench** editor, a dedicated workspace for scene layout and entity manipulation, like Godot's node hierarchy meets the workflow and grit of Source/Hammer.

### Current State
This is an active development project. The foundation is being built, focusing on a robust scene-node architecture and a high-UX editor.

* **Single-Project Architecture:** Everything (Engine + Wrench Editor) lives in one place.
* **Node Hierarchy:** ParentChild transform inheritance.
* **Wrench UI:** Live viewport, hierarchy management, and property inspector.
* **Assimp Integration:** Built to handle models (OBJ/FBX)


### Roadmap
- [x] OpenGL 4.1 Core Context
- [x] Wrench Editor Shell & Framebuffer
- [ ] Node-based Scene Graph
- [ ] FPS-style Viewport Camera
- [ ] Mouse Picking / Selection
- [ ] PBR-lite Shaders

### 🛠️ Getting Started
Visual Engine uses CMake and vcpkg in Manifest Mode.

# Prerequisities
    Visual Studio 2022+
    C++20 compiler.
    vcpkg installed and is available in your PATH.
    CMake 3.21+.
    
# Clone the repository
```
git clone https://github.com/VisualBridge/VisualEngine.git
cd VisualEngine
```

# Build via CommandLine
Replace <VCPKG_PATH> with your actual vcpkg directory.
```
cmake -S . -B build `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="<VCPKG_PATH>/scripts/buildsystems/vcpkg.cmake" `
  -DVCPKG_TARGET_TRIPLET=x64-windows

cmake --build build --config x64-Debug
./out/build/x64-Debug/VisualEngine.exe
```

# Build via Visual Studio
Open the repository folder in Visual Studio and click **ctrl shift b** on your keyboard.




**Disclaimer:** This isn't a "release" yet. It’s a work-in-progress for those who like raw C++ and building tools from scratch.
