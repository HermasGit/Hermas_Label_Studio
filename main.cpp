#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


// ...existing code...

// Texture structure for slideshow images
struct SlideImage {
    GLuint id = 0;
    int width = 0;
    int height = 0;
};

SlideImage slideImages[4];
int currentSlide = 0;
float slideTimer = 0.0f;
const float slideInterval = 3.0f; // seconds
#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// ...existing code...

// Windows API for taskbar control
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// Texture structure for icons
struct IconTexture {
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
};

IconTexture minimizeIcon, maximizeIcon, restoreIcon, closeIcon;

// Font variables
ImFont* poppinsFont = nullptr;
ImFont* poppinsTitleFont = nullptr;

// Window state
bool isMaximized = false;

// Function to ALWAYS center window - NO EXCEPTIONS
void CenterWindowAlways(GLFWwindow* window) {
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    
    int monitorX, monitorY;
    glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    
    // FORCE ABSOLUTE CENTER - no compromises
    int centerX = monitorX + (mode->width - windowWidth) / 2;
    int centerY = monitorY + (mode->height - windowHeight) / 2;
    
    glfwSetWindowPos(window, centerX, centerY);
}

#ifdef _WIN32
// Functions to control true fullscreen behavior
void SetTrueFullscreen(GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd) {
        // Get monitor dimensions including taskbar area
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        
        // Set window to cover ENTIRE screen including taskbar area
        // Use HWND_TOPMOST with SWP_NOACTIVATE to prevent focus loss
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, mode->width, mode->height, 
                    SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOACTIVATE);
    }
}

void ExitTrueFullscreen(GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd) {
        // Remove topmost and restore normal window behavior
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, 
                    SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
}
#endif

// Function to load PNG image as OpenGL texture
bool LoadIconTexture(const char* filename, IconTexture* icon) {
    int channels;
    unsigned char* data = stbi_load(filename, &icon->width, &icon->height, &channels, 4);
    if (data == nullptr) {
        return false;
    }

    glGenTextures(1, &icon->textureID);
    glBindTexture(GL_TEXTURE_2D, icon->textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, icon->width, icon->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
    return true;
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void SetupCustomStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Beautiful white theme with blue accents
    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);  // COMPLETELY TRANSPARENT BORDERS
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);  // NO BORDER SHADOW
    colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]             = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.76f, 0.80f, 0.84f, 0.93f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.60f, 0.73f, 0.88f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight]          = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    style.PopupRounding = 0;  // NO ROUNDING ANYWHERE
    style.WindowPadding = ImVec2(0, 0);  // NO WINDOW PADDING
    style.FramePadding  = ImVec2(0, 0);  // NO FRAME PADDING
    style.ItemSpacing   = ImVec2(0, 0);  // NO ITEM SPACING
    style.ItemInnerSpacing = ImVec2(0, 0);  // NO INNER SPACING
    style.TouchExtraPadding = ImVec2(0, 0);  // NO TOUCH PADDING
    style.IndentSpacing = 0;  // NO INDENT SPACING
    style.ColumnsMinSpacing = 0;  // NO COLUMN SPACING
    style.ScrollbarSize = 18;
    style.WindowBorderSize = 0;  // NO BORDERS ANYWHERE
    style.ChildBorderSize  = 0;  // NO CHILD BORDERS
    style.PopupBorderSize  = 0;  // NO POPUP BORDERS
    style.FrameBorderSize  = 0;  // NO FRAME BORDERS
    style.WindowRounding    = 0;  // PERFECT RECTANGLE - no rounded corners
    style.ChildRounding     = 0;  // PERFECT RECTANGLE - no rounded child corners
    style.FrameRounding     = 0;  // NO FRAME ROUNDING
    style.ScrollbarRounding = 0;  // NO SCROLLBAR ROUNDING
    style.GrabRounding      = 0;  // NO GRAB ROUNDING
}

void RenderCustomTitleBar(GLFWwindow* window)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 40));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                   ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // Push styles
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 1.00f));
    
    bool open = true;
    if (ImGui::Begin("##titlebar", &open, window_flags))
    {
        float windowWidth = ImGui::GetWindowWidth();
        
        // Title text (centered)
        const char* title = "HERMAS LABEL STUDIO";
        ImVec2 textSize = ImGui::CalcTextSize(title);
        float centerX = (windowWidth - textSize.x) * 0.5f;
        if (poppinsTitleFont) {
            ImGui::PushFont(poppinsTitleFont);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White text
        ImGui::SetCursorPosX(centerX);
        ImGui::Text(title);
        ImGui::PopStyleColor();
        if (poppinsTitleFont) {
            ImGui::PopFont();
        }
        
        // Window controls (right side)
        float buttonWidth = 46.0f;
        float buttonHeight = 40.0f;
        float totalButtonsWidth = buttonWidth * 3; // 3 buttons
        ImGui::SetCursorPos(ImVec2(windowWidth - totalButtonsWidth, 0));
        
        // Button styles for professional appearance
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White text
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        
        // Window controls (no Marlett font)
        // Minimize button
        if (ImGui::Button("_", ImVec2(buttonWidth, buttonHeight))) {
            glfwIconifyWindow(window);
        }

        ImGui::SameLine();

        // Maximize/Restore button
        if (ImGui::Button(isMaximized ? "[]" : "[ ]", ImVec2(buttonWidth, buttonHeight))) {
            if (isMaximized) {
                glfwRestoreWindow(window);
                CenterWindowAlways(window);
                isMaximized = false;
            } else {
                glfwMaximizeWindow(window);
                isMaximized = true;
            }
        }

        ImGui::SameLine();

        // Close button
        if (ImGui::Button("X", ImVec2(buttonWidth, buttonHeight))) {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::PopStyleColor(4); // Pop button colors and text color
        ImGui::PopStyleVar();    // Pop ItemSpacing
    }
    ImGui::End();
    
    // Pop all styles in reverse order
    ImGui::PopStyleColor(); // Pop window bg
    ImGui::PopStyleVar(3);  // Pop window styles
}

int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Remove default title bar

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hermas Label Studio", nullptr, nullptr);
    if (window == nullptr)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    // START IN FULLSCREEN (MAXIMIZED) BY DEFAULT - like professional software
    glfwMaximizeWindow(window);
    isMaximized = true;  // Set the state to maximized
    
    // Set callback to center window when restored from minimize
    glfwSetWindowIconifyCallback(window, [](GLFWwindow* window, int iconified) {
        if (!iconified) {
            CenterWindowAlways(window);  // ALWAYS CENTER AFTER MINIMIZE/RESTORE
        }
    });

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Load Poppins font
    poppinsFont = io.Fonts->AddFontFromFileTTF("../../fonts/Poppins-Regular.ttf", 16.0f);
    poppinsTitleFont = io.Fonts->AddFontFromFileTTF("../../fonts/Poppins-Regular.ttf", 20.0f);
    
    
    // Fallback to default if font loading fails
    if (!poppinsFont) {
        poppinsFont = io.Fonts->AddFontDefault();
    }
    if (!poppinsTitleFont) {
        poppinsTitleFont = io.Fonts->AddFontDefault();
    }
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup custom style
    SetupCustomStyle();
    
    // Load icon textures with error reporting
    // Load slideshow images from 'slides' folder
    const char* slidePaths[4] = {
        "../../slides/welcome_window_herbal_1.jpg",
        "../../slides/welcome_window_herbal_2.jpg",
        "../../slides/welcome_window_herbal_3.jpg",
        "../../slides/welcome_window_herbal_4.jpg"
    };
    for (int i = 0; i < 4; ++i) {
        int channels;
        unsigned char* data = stbi_load(slidePaths[i], &slideImages[i].width, &slideImages[i].height, &channels, 4);
        if (data) {
            GLuint texId = 0;
            glGenTextures(1, &texId);
            glBindTexture(GL_TEXTURE_2D, texId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, slideImages[i].width, slideImages[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            slideImages[i].id = texId;
            stbi_image_free(data);
        } else {
            slideImages[i].id = 0;
        }
    }
    if (!LoadIconTexture("../../icons/minimize.png", &minimizeIcon)) {
        printf("Warning: Failed to load minimize.png icon\n");
    } else {
        printf("Successfully loaded minimize.png icon\n");
    }
    
    if (!LoadIconTexture("../../icons/maximize.png", &maximizeIcon)) {
        printf("Warning: Failed to load maximize.png icon\n");
    } else {
        printf("Successfully loaded maximize.png icon\n");
    }
    
    if (!LoadIconTexture("../../icons/restore.png", &restoreIcon)) {
        printf("Warning: Failed to load restore.png icon\n");
    } else {
        printf("Successfully loaded restore.png icon\n");
    }
    
    if (!LoadIconTexture("../../icons/close.png", &closeIcon)) {
        printf("Warning: Failed to load close.png icon\n");
    } else {
        printf("Successfully loaded close.png icon\n");
    }

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Handle keyboard shortcuts - F11 for fullscreen toggle
        if (ImGui::IsKeyPressed(ImGuiKey_F11)) {
            if (isMaximized) {
                glfwRestoreWindow(window);
                CenterWindowAlways(window);  // ALWAYS CENTER AFTER RESTORE
                isMaximized = false;
            } else {
                glfwMaximizeWindow(window);
                isMaximized = true;
            }
        }
        
        // Set default font for the frame
        if (poppinsFont) {
            ImGui::PushFont(poppinsFont);
        }

        // Render custom title bar
        RenderCustomTitleBar(window);

        // Update slideshow timer
        slideTimer += ImGui::GetIO().DeltaTime;
        if (slideTimer >= slideInterval) {
            slideTimer = 0.0f;
            currentSlide = (currentSlide + 1) % 4;
        }

        // Demo main window with WHITE background for 40%/60% split
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // WHITE background for proper 40%/60% split
        ImGui::SetNextWindowPos(ImVec2(0, 40)); // Below title bar
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - 40));
        
        ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                     ImGuiWindowFlags_NoBringToFrontOnFocus;
        
        if (ImGui::Begin("##MainWindow", nullptr, main_flags))
        {
            // Get the available content area size
            ImVec2 contentSize = ImGui::GetContentRegionAvail();
            
            // Calculate 40% and 60% widths
            float leftWidth = contentSize.x * 0.4f;
            float rightWidth = contentSize.x * 0.6f;
            
            // Push style to remove borders completely
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            
            // LEFT SIDE - 40% BLACK PLACEHOLDER AREA
            ImGui::BeginChild("##LeftPanel", ImVec2(leftWidth, contentSize.y), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
            {
                // Fill the entire left area with black color
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 leftPanelMin = ImGui::GetWindowPos();
                ImVec2 leftPanelMax = ImVec2(leftPanelMin.x + leftWidth, leftPanelMin.y + contentSize.y);
                drawList->AddRectFilled(leftPanelMin, leftPanelMax, IM_COL32(0, 0, 0, 255)); // Black color

                // Draw the current slide image centered/scaled
                if (slideImages[currentSlide].id) {
                    float panelW = leftWidth;
                    float panelH = contentSize.y;
                    float imgW = (float)slideImages[currentSlide].width;
                    float imgH = (float)slideImages[currentSlide].height;
                    float scale = std::min(panelW / imgW, panelH / imgH);
                    ImVec2 imgSize = ImVec2(imgW * scale, imgH * scale);
                    ImVec2 imgPos = ImVec2(
                        leftPanelMin.x + (panelW - imgSize.x) * 0.5f,
                        leftPanelMin.y + (panelH - imgSize.y) * 0.5f
                    );
                    drawList->AddImage((void*)(intptr_t)slideImages[currentSlide].id, imgPos, ImVec2(imgPos.x + imgSize.x, imgPos.y + imgSize.y));
                }
            }
            ImGui::EndChild();
            
            // Move to the right side
            ImGui::SameLine();
            
            // RIGHT SIDE - 60% WHITE CONTENT AREA
            ImGui::BeginChild("##RightPanel", ImVec2(rightWidth, contentSize.y), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
            {
                // This is the main white content area (60% of the window)
                // Future content will go here
            }
            ImGui::EndChild();
            
            // Pop the style changes
            ImGui::PopStyleVar(2);
        }
        ImGui::End();
        ImGui::PopStyleColor(); // Pop the black window background

        // Pop the default font
        if (poppinsFont) {
            ImGui::PopFont();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.95f, 0.95f, 0.95f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
