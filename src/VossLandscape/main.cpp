#include "stdafx.h"

#include "GlUtils.h"
#include "GlShader.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "LogFormatter.h"

#include "MathFunctions.h"
#include "Image.h"
#include "GlImage.h"
#include "VossHeightmap.h"
#include "HeightmapRender.h"

#include "ScopeGuard.h"

static const size_t g_Width = 800;
static const size_t g_Height = 600;
static const char g_Title[] = "Voss Heightmap Generator";

static const size_t g_LandscapeSize = 7; // 2^7 = 128x128 heightmap

/*****************************************************************************
 * Data structures
 ****************************************************************************/
/* Screen triangle vertices */
static const int g_VerticesCount = 6;
static const GLfloat g_Vertices[] = {
    -1.f, -1.f,  0.0f, 0.0f
    , 1.f, -1.f,  1.0f, 0.0f
    , 1.f,  1.f,  1.0f, 1.0f

    , -1.f, -1.f,  0.0f, 0.0f
    ,  1.f,  1.f,  1.0f, 1.0f
    , -1.f,  1.f,  0.0f, 1.0f
};


/*****************************************************************************
 * Vec2
 ****************************************************************************/

struct Vec2
{
    int x, y;
};

bool operator!=(const Vec2& lhs, const Vec2& rhs)
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

/*****************************************************************************
 * SceneContext
 ****************************************************************************/

#define CONTEXT_CB(window,context) \
    assert(window); \
    SceneContext* context = static_cast<SceneContext*>(glfwGetWindowUserPointer(window)); \
    assert(context); \

#define CONTEXT_GLFW_CB_2(fn, p1, p2) \
public: \
    static void fn ## Cb(GLFWwindow* window, int p1, int p2) { \
        CONTEXT_CB(window,pContext) \
        pContext->fn(p1, p2); \
    } \
private: \
    void fn(int p1, int p2);

#define CONTEXT_GLFW_CB_4(fn, p1, p2, p3, p4) \
public: \
    static void fn ## Cb(GLFWwindow* window, int p1, int p2, int p3, int p4) { \
        CONTEXT_CB(window,pContext) \
        pContext->fn(p1, p2, p3, p4); \
    } \
private: \
    void fn(int p1, int p2, int p3, int p4);

class SceneContext {
public:
    SceneContext();
    ~SceneContext();

    bool Init(GLFWwindow* window);
    void Release();

    void Display();
    void Update();

    CONTEXT_GLFW_CB_2(Reshape, width, height)
    CONTEXT_GLFW_CB_4(Keyboard, key, scancode, action, mode)

private:
    void DisplayUI();

private:
    GLFWwindow* mWindow = nullptr;
    int mWindowWidth = 0, mWindowHeight = 0;
    bool mFullscreen = false;
    int mSavedXPos = 0, mSavedYPos = 0;
    int mSavedWidth = 0, mSavedHeight = 0;

    ShaderProgram program;
    VertexBuffer pointsVbo;
    VertexArray pointsVao;

    int landscapeSize = g_LandscapeSize;
    Vec2 resolution = { g_Width, g_Height };
    Vec2 worldPos = { 10, 10 };
    double heightSeek = 2.0, slopeSeek = 2200.0;

    std::unique_ptr<VossHeightmap> map;
    std::unique_ptr<GlImage> image;
    std::unique_ptr<HeightmapRender> render;
};

SceneContext::SceneContext()
    : pointsVbo(GL_TRIANGLES, g_VerticesCount, g_Vertices)
    , pointsVao(program) {
}

SceneContext::~SceneContext() {
    Release();
}

bool SceneContext::Init(GLFWwindow* window) {
    LOGI << "OpenGL Renderer  : " << glGetString(GL_RENDERER);
    LOGI << "OpenGL Vendor    : " << glGetString(GL_VENDOR);
    LOGI << "OpenGL Version   : " << glGetString(GL_VERSION);
    LOGI << "GLSL Version     : " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    mWindow = window;
    glfwSetWindowUserPointer(mWindow, static_cast<void *>(this));

    glfwGetWindowSize(mWindow, &mWindowWidth, &mWindowHeight);

    map = std::make_unique<VossHeightmap>(landscapeSize);
    image = std::make_unique<GlImage>(resolution.x, resolution.y);
    render = std::make_unique<HeightmapRender>(map.get(), image.get());

    map->SetSeekValues(heightSeek, slopeSeek);
    map->generate(worldPos.x, worldPos.y);

    return true;
}

void SceneContext::Release() {
    //
}

void SceneContext::Display() {
    glClear(GL_COLOR_BUFFER_BIT); LOGOPENGLERROR();

    program.UseProgram();

    image->BindTexture();

    program.SetUniforms(mWindowWidth, mWindowHeight);

    pointsVbo.BindBuffer();

    pointsVao.BindVertexArray();

    render->draw();
    image->CopyToTexture();

    pointsVbo.DrawArrays();

    pointsVao.UnbindVertexArray();

    DisplayUI();
}

void SceneContext::DisplayUI() {
    Vec2 newWorldPos = worldPos;
    Vec2 newResolution = resolution;
    float newHeightSeek = heightSeek;
    float newSlopeSeek = slopeSeek;
    int newLandscapeSize = landscapeSize;

    ImGui::Begin("Heightmap Settings");

    ImGui::Text("Heightmap Size");

    ImGui::BeginGroup();
    if (ImGui::Button("64x64")) {
        newLandscapeSize = 6;
    }
    ImGui::SameLine();
    if (ImGui::Button("128x128")) {
        newLandscapeSize = 7;
    }
    ImGui::SameLine();
    if (ImGui::Button("256x256")) {
        newLandscapeSize = 8;
    }
    ImGui::EndGroup();

    ImGui::Text("Output Resolution");

    if (ImGui::Button("640x480")) {
        newResolution = { 640, 480 };
    }
    ImGui::SameLine();
    if (ImGui::Button("800x600")) {
        newResolution = { 800, 600 };
    }
    ImGui::SameLine();
    if (ImGui::Button("1024x768")) {
        newResolution = { 1024, 768 };
    }

    ImGui::InputFloat("Height Seek", &newHeightSeek, 0.1f, 1.0f, "%.1f");

    ImGui::InputFloat("Slope Seek", &newSlopeSeek, 50.f, 100.0f, "%.0f");

    ImGui::Text("World Position: %d, %d", worldPos.x, worldPos.y);

    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
        newWorldPos.x--;
    }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
        newWorldPos.x++;
    }
    //ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
        newWorldPos.y--;
    }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("##down", ImGuiDir_Down)) {
        newWorldPos.y++;
    }

    if (worldPos != newWorldPos
        || heightSeek != newHeightSeek
        || slopeSeek != newSlopeSeek) {
        worldPos = newWorldPos;
        heightSeek = newHeightSeek;
        slopeSeek = newSlopeSeek;
        LOGI << "Move to " << worldPos.x << "," << worldPos.y;
        LOGI << "Set heightmap parameters to "
            << "heightSeek=" << heightSeek
            << " slopeSeek=" << slopeSeek;

        map->SetSeekValues(heightSeek, slopeSeek);

        map->generate(worldPos.x, worldPos.y);

        image->clear();
    }

    if (resolution != newResolution) {
        resolution = newResolution;
        LOGI << "Set image resolution to " << resolution.x << "," << resolution.y;
        image.reset(new GlImage(resolution.x, resolution.y));
        render.reset(new HeightmapRender(map.get(), image.get()));
    }

    if (landscapeSize != newLandscapeSize) {
        landscapeSize = newLandscapeSize;
        LOGI << "Set landscape size to " << landscapeSize;

        map.reset(new VossHeightmap(landscapeSize));

        map->SetSeekValues(heightSeek, slopeSeek);
        map->generate(worldPos.x, worldPos.y);

        image->clear();

        render.reset(new HeightmapRender(map.get(), image.get()));
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)"
        , 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void SceneContext::Reshape(int width, int height) {
    glViewport(0, 0, width, height); LOGOPENGLERROR();
    mWindowWidth = width;
    mWindowHeight = height;
}

void SceneContext::Keyboard(int key, int /*scancode*/, int action, int /*mode*/) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
            break;

        case GLFW_KEY_F1:
            mFullscreen = !mFullscreen;
            if (mFullscreen) {
                glfwGetWindowPos(mWindow, &mSavedXPos, &mSavedYPos);
                glfwGetWindowSize(mWindow, &mSavedWidth, &mSavedHeight);

                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(mWindow, monitor, 0, 0,
                    mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(mWindow, nullptr, mSavedXPos, mSavedYPos,
                    mSavedWidth, mSavedHeight, GLFW_DONT_CARE);
            }
            break;
        }
    }
}

void SceneContext::Update() {
    //
}

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

    glfwSetKeyCallback(window.get(), SceneContext::KeyboardCb);
    glfwSetInputMode(window.get(), GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwSetWindowSizeCallback(window.get(), SceneContext::ReshapeCb);

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
    SceneContext context;
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
int main(int argc, const char* argv[])
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
