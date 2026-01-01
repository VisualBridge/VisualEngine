# 🛠VisualEngine

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
1. Clone the repository.
2. Open `VisualEngine.slnx` in Visual Studio 2022 or Visual Studio 2026.
3. Build and Run.

**Disclaimer:** This isn't a "release" yet. It’s a work-in-progress for those who like raw C++ and building tools from scratch.
