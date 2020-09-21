#include "stdafx.h"
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
#include "HeightmapRender.h"

#include "ScopeGuard.h"
#include "LandscapeContext.h"

static const int g_VerticesCount = 6;
static const GLfloat g_Vertices[] = {
    -1.f, -1.f,  0.0f, 0.0f
    , 1.f, -1.f,  1.0f, 0.0f
    , 1.f,  1.f,  1.0f, 1.0f

    , -1.f, -1.f,  0.0f, 0.0f
    ,  1.f,  1.f,  1.0f, 1.0f
    , -1.f,  1.f,  0.0f, 1.0f
};

static const std::vector<linearalg::ivec2> ResolutionData = {
    {640, 480},
    {800, 600},
    {1024, 768}
};

LandscapeContext::LandscapeContext()
    : pointsVbo(GL_TRIANGLES, g_VerticesCount, g_Vertices)
    , pointsVao(program) {
}

LandscapeContext::~LandscapeContext() {
    Release();
}

bool LandscapeContext::Init(GLFWwindow* window) {
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

void LandscapeContext::Release() {
    //
}

void LandscapeContext::UpdateRenderedLandscape() {
    render->draw();
    image->CopyToTexture();
}

void LandscapeContext::Display() {
    glClear(GL_COLOR_BUFFER_BIT); LOGOPENGLERROR();

    program.UseProgram();

    image->BindTexture();

    program.SetUniforms(mWindowWidth, mWindowHeight);

    pointsVbo.BindBuffer();

    pointsVao.BindVertexArray();

    pointsVbo.DrawArrays();

    pointsVao.UnbindVertexArray();

    DisplayUI();
}

void LandscapeContext::DisplayUI() {
    linearalg::ivec2 newWorldPos = worldPos;
    int newResolutionId = resolutionId;
    float newHeightSeek = heightSeek;
    int newLandscapeSize = landscapeSize;

    ImGui::Begin("Heightmap Settings");

    ImGui::Text("Procedural Landscape");

    ImGui::Separator();

    ImGui::Text("Heightmap Size");

    static const std::vector<std::tuple<std::string, int>> LandscapeSizeData = {
        {"64x64", 6}, {"128x128", 7}, {"256x256", 8},
    };

    bool firstItemFlag = true;
    for (const auto& data : LandscapeSizeData) {
        std::string name;
        int size;
        std::tie(name, size) = data;

        if (firstItemFlag) {
            firstItemFlag = false;
        }
        else {
            ImGui::SameLine();
        }

        ImGui::RadioButton(name.c_str(), &newLandscapeSize, size);
    }

    ImGui::Separator();

    ImGui::Text("Output Resolution");

    static const std::vector<std::tuple<std::string, int>> OutputResolutionData = {
        {"640x480", 0}, {"800x600", 1}, {"1024x768", 2},
    };

    firstItemFlag = true;
    for (const auto& data : OutputResolutionData) {
        std::string name;
        int sizeId;
        std::tie(name, sizeId) = data;

        if (firstItemFlag) {
            firstItemFlag = false;
        }
        else {
            ImGui::SameLine();
        }

        ImGui::RadioButton(name.c_str(), &newResolutionId, sizeId);
    }

    ImGui::Separator();

    ImGui::SliderFloat("Height Seek", &newHeightSeek, 1.4f, 3.9f, "%.1f");

    ImGui::Separator();

    ImGui::Text("World Navigation");
    ImGui::Text("Position: %d, %d", worldPos.x, worldPos.y);

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
        || heightSeek != newHeightSeek) {
        worldPos = newWorldPos;
        heightSeek = newHeightSeek;
        LOGI << "Move to " << worldPos.x << "," << worldPos.y;
        LOGI << "Set heightmap parameters to "
            << "heightSeek=" << heightSeek;

        map->SetSeekValues(heightSeek, slopeSeek);

        map->generate(worldPos.x, worldPos.y);

        image->clear();

        mIsLandscapeNeedsUpdate = true;
    }

    if (resolutionId != newResolutionId) {
        resolutionId = newResolutionId;
        resolution = ResolutionData[resolutionId];
        LOGI << "Set image resolution to " << resolution.x << "," << resolution.y;
        image.reset(new GlImage(resolution.x, resolution.y));
        render->setImage(image.get());

        mIsLandscapeNeedsUpdate = true;
    }

    if (landscapeSize != newLandscapeSize) {
        landscapeSize = newLandscapeSize;
        LOGI << "Set landscape size to " << landscapeSize;

        map.reset(new VossHeightmap(landscapeSize));

        map->SetSeekValues(heightSeek, slopeSeek);
        map->generate(worldPos.x, worldPos.y);

        image->clear();

        render.reset(new HeightmapRender(map.get(), image.get()));

        mIsLandscapeNeedsUpdate = true;
    }

    ImGui::Separator();

    ImGui::Text("User guide:");
    ImGui::BulletText("F1 to toggle fullscreen");
    ImGui::BulletText("ESC to exit");

    ImGui::Separator();

    ImGui::Text("FPS Counter: %.1f", mFps);

    ImGui::End();
}

void LandscapeContext::Reshape(int width, int height) {
    glViewport(0, 0, width, height); LOGOPENGLERROR();
    mWindowWidth = width;
    mWindowHeight = height;
}

void LandscapeContext::Keyboard(int key, int /*scancode*/, int action, int /*mode*/) {
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

void LandscapeContext::Update() {
    static double lastFpsTime = 0.0;
    double currentTime = glfwGetTime();

    if (currentTime - lastFpsTime > 1.0) {
        mFps = ImGui::GetIO().Framerate;
        lastFpsTime = currentTime;
    }

    if (mIsLandscapeNeedsUpdate) {
        UpdateRenderedLandscape();
    }
}
