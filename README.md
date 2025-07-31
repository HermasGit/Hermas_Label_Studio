# Hermas Label Studio

A robust, beautiful, and elegant C++ desktop application built with Dear ImGui + GLFW.

## Features

✨ **Custom Title Bar** - Beautiful, professional title bar with window controls
🎨 **Modern Dark Theme** - Elegant blue accent color scheme
🏗️ **Robust Architecture** - Clean, organized code structure
⚡ **High Performance** - Native C++ performance with OpenGL rendering
🔧 **Easy Setup** - Simple build process with CMake

## Requirements

- C++17 compatible compiler
- CMake 3.10+
- OpenGL 3.0+
- GLFW3
- Dear ImGui (included as submodule)

## Quick Setup

### Simple 3-Step Setup ⚡
```bash
# 1. Clone required libraries (done!)
git clone https://github.com/glfw/glfw.git
git clone https://github.com/ocornut/imgui.git

# 2. Build the project
mkdir build
cd build
cmake ..

# 3. Compile and run
cmake --build . --config Release
```

That's it! No complex installations needed.

## Project Structure

```
HermasLabelStudio/
├── main.cpp              # Main application file
├── CMakeLists.txt         # Build configuration
├── imgui/                 # Dear ImGui library
└── build/                 # Build output
```

## Title Bar Features

🎯 **Custom Window Controls**
- Minimize, Maximize/Restore, Close buttons
- Professional hover effects
- Red close button for better UX

🏷️ **Branded Header**
- Application logo and name
- Custom blue accent color
- Clean, modern typography

## Architecture

The application uses a clean, modular structure:

- **Custom Style System** - Professional dark theme with blue accents
- **Title Bar Manager** - Handles window controls and branding
- **Main Window System** - Organized content areas
- **Event Handling** - Robust GLFW integration

## Next Steps

1. Add more windows (Settings, Project Manager, etc.)
2. Implement file operations
3. Add menu system
4. Create dialog boxes
5. Add status bar

Ready to build amazing label studio software! 🚀
