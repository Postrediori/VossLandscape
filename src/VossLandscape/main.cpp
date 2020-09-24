#include "pch.h"

#include "LinearAlg.h"
#include "GlUtils.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "LogFormatter.h"

#include "MathFunctions.h"
#include "Image.h"
#include "GlImage.h"
#include "VossHeightmap.h"
#include "IHeightmapRenderer.h"
#include "MinimapRenderer.h"
#include "HeightmapRender.h"

#include "ScopeGuard.h"
#include "LandscapeContext.h"

static const size_t g_Width = 800;
static const size_t g_Height = 600;
static const char g_Title[] = "Voss Heightmap Generator";


/*****************************************************************************
 * Run Scene
 ****************************************************************************/
void Error(int /*error*/, const char* description) {
    LOGE << "Error: " << description;
}

bool RunScene() {
    glfwSetErrorCallback(Error);

    if (!glfwInit()) {
        LOGE << "Failed to load GLFW";
        return false;
    }
    ScopeGuard glfwGuard([]() { glfwTerminate(); });

    LOGI << "Init window context with OpenGL 3.3 Core Profile";
    static const char* gGlslVersion = "#version 330 core";
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window(
        glfwCreateWindow(g_Width, g_Height, g_Title, nullptr, nullptr),
        &glfwDestroyWindow);
    if (!window) {
        LOGE << "Unable to Create OpenGL 3.3 Core Profile Context";
        return false;
    }

    glfwSetKeyCallback(window.get(), LandscapeContext::KeyboardCb);
    glfwSetInputMode(window.get(), GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwSetWindowSizeCallback(window.get(), LandscapeContext::ReshapeCb);

    glfwMakeContextCurrent(window.get());
    gladLoadGL();

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable .ini

    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init(gGlslVersion);
    ScopeGuard imguiGuard([]() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    });

    // Init program objects
    LandscapeContext context;
    if (!context.Init(window.get())) {
        LOGE << "Initialization failed";
        return false;
    }

    while (!glfwWindowShouldClose(window.get())) {
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

        glfwSwapBuffers(window.get());
    }

    return true;
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

    try {
        bool status = RunScene();
        if (!status) {
            LOGE << "Error occured while running the scene";
            return -1;
        }
    }
    catch (const std::exception& ex) {
        LOGE << ex.what();
        return -1;
    }
    catch (...) {
        LOGE << "Unknown exception happened";
        return -1;
    }

    return 0;
}
