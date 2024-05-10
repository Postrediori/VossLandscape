#include "pch.h"

#include "LinearAlg.h"
#include "GraphicsLogger.h"
#include "Shader.h"
#include "LogFormatter.h"

#include "MathFunctions.h"
#include "VossHeightmap.h"
#include "MinimapRenderer.h"
#include "HeightmapRender.h"
#include "GraphicsResource.h"
#include "GraphicsShape.h"
#include "TextureImage.h"

#include "LandscapeContext.h"

constexpr size_t g_Width = 800;
constexpr size_t g_Height = 600;
const std::string g_Title = "Voss Heightmap Generator";


/*****************************************************************************
 * GLFW Callbacks
 ****************************************************************************/

void ErrorCallback(int code, const char* description) {
    auto const codeStr = [code]() {
        switch (code) {
        case GLFW_NOT_INITIALIZED: return "NOT_INITIALIZED";
        case GLFW_NO_CURRENT_CONTEXT: return "NO_CURRENT_CONTEXT";
        case GLFW_INVALID_ENUM: return "INVALID_ENUM";
        case GLFW_INVALID_VALUE: return "INVALID_VALUE";
        case GLFW_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case GLFW_API_UNAVAILABLE: return "API_UNAVAILABLE";
        case GLFW_VERSION_UNAVAILABLE: return "VERSION_UNAVAILABLE";
        case GLFW_PLATFORM_ERROR: return "PLATFORM_ERROR";
        case GLFW_FORMAT_UNAVAILABLE: return "FORMAT_UNAVAILABLE";
        case GLFW_NO_WINDOW_CONTEXT: return "NO_WINDOW_CONTEXT";
        default: return "Unknown error";
        }
    }();

    LOGE << "GLFW Error " << codeStr << " : " << description;
}


/*****************************************************************************
 * Main
 ****************************************************************************/

int main(int /*argc*/, const char** /*argv*/)
{
    plog::ConsoleAppender<plog::LogFormatter> consoleAppender;
#ifdef NDEBUG
    plog::init(plog::info, &consoleAppender);
#else
    plog::init(plog::debug, &consoleAppender);
#endif

    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit()) {
        LOGE << "Failed to load GLFW";
        return -1;
    }

    LOGI << "Init window context with OpenGL 3.3 Core Profile";
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    auto window = glfwCreateWindow(g_Width, g_Height, g_Title.c_str(), nullptr, nullptr);
    if (!window) {
        LOGE << "Unable to Create OpenGL 3.3 Core Profile Context";
        return -1;
    }

    glfwSetKeyCallback(window, LandscapeContext::KeyboardCb);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwSetWindowSizeCallback(window, LandscapeContext::ReshapeCb);

    glfwMakeContextCurrent(window);
    gladLoadGL();

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable .ini

    static const char glslVersion[] = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // Init program objects
    int status = 0;
    LandscapeContext context;
    if (!context.Init(window)) {
        LOGE << "Initialization failed";
        status = -1;
        goto finish;
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render scene
        context.Display();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update objects
        context.Update();

        glfwSwapBuffers(window);
    }

finish:
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return status;
}
