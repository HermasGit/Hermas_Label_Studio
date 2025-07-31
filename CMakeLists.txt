cmake_minimum_required(VERSION 3.10)
project(HermasLabelStudio)

set(CMAKE_CXX_STANDARD 17)

# Find OpenGL
find_package(OpenGL REQUIRED)

# GLFW - Simple approach, build from source
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add GLFW subdirectory (we'll clone it)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt")
    add_subdirectory(glfw)
else()
    message(STATUS "GLFW not found. Please run: git clone https://github.com/glfw/glfw.git")
    message(FATAL_ERROR "Missing GLFW. Clone it with: git clone https://github.com/glfw/glfw.git")
endif()

# Include directories for Dear ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)

# Dear ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Create executable
add_executable(HermasLabelStudio
    main.cpp
    ${IMGUI_SOURCES}
)

# Link libraries
target_link_libraries(HermasLabelStudio
    OpenGL::GL
    glfw
)

# Windows specific settings
if(WIN32)
    target_link_libraries(HermasLabelStudio opengl32)
endif()

# Set startup project for Visual Studio
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HermasLabelStudio)
endif()
