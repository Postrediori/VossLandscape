#include "stdafx.h"
#include "LinearAlg.h"
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

void LandscapeContext::Display() {
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

void LandscapeContext::DisplayUI() {
    linearalg::ivec2 newWorldPos = worldPos;
    linearalg::ivec2 newResolution = resolution;
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
    //
}
