# NetworkGamePractice

This project is a practice for learning network programming and multithreading in C++ by implementing a simple 2D game.

## Features
- OpenGL-based rendering (planned)
- Networked game features with own dedicated local server (planned)
- Multithreaded game logic and rendering (planned)

## Goals
- Develop a simple but functional 2D game as a learning project
- Explore custom networking systems
- Experiment with multithreading techniques

## Build Instructions
1. Create a build directory and configure the project
```bash
cd Client
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
2. Run the executable
```bash
cd Release
./Client.exe
```

## Dependencies
- GLEW: Precompiled binaries included in lib/.
- GLFW: Precompiled binaries included in lib/.