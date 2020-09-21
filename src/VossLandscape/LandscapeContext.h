#pragma once

#define CONTEXT_CB(window,context) \
    assert(window); \
    LandscapeContext* context = static_cast<LandscapeContext*>(glfwGetWindowUserPointer(window)); \
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

class LandscapeContext {
public:
    static const size_t g_LandscapeSize = 7; // 2^7 = 128x128 heightmap

    LandscapeContext();
    ~LandscapeContext();

    bool Init(GLFWwindow* window);
    void Release();

    void Display();
    void Update();

    CONTEXT_GLFW_CB_2(Reshape, width, height)
    CONTEXT_GLFW_CB_4(Keyboard, key, scancode, action, mode)

private:
    void DisplayUI();
    void UpdateRenderedLandscape();

private:
    GLFWwindow* mWindow = nullptr;
    int mWindowWidth = 0, mWindowHeight = 0;
    bool mFullscreen = false;
    int mSavedXPos = 0, mSavedYPos = 0;
    int mSavedWidth = 0, mSavedHeight = 0;
    float mFps = 0.f;
    bool mIsLandscapeNeedsUpdate = true;

    ShaderProgram program;
    VertexBuffer pointsVbo;
    VertexArray pointsVao;

    int landscapeSize = g_LandscapeSize;
    int resolutionId = 1;
    linearalg::ivec2 resolution = { 800, 600 };
    linearalg::ivec2 worldPos = { 10, 10 };
    double heightSeek = 2.0, slopeSeek = 2200.0;

    std::unique_ptr<VossHeightmap> map;
    std::unique_ptr<GlImage> image;
    std::unique_ptr<HeightmapRender> render;
    
    std::unique_ptr<GlImage> minimapImage;
    std::unique_ptr<MinimapRenderer> minimapRender;
};
