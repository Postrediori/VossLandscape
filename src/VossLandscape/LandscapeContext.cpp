#include "pch.h"
#include "LinearAlg.h"
#include "GraphicsLogger.h"
#include "Shader.h"
#include "LogFormatter.h"

#include "MathFunctions.h"
#include "VossHeightmap.h"
#include "HeightmapRender.h"
#include "MinimapRenderer.h"
#include "GraphicsResource.h"
#include "GraphicsShape.h"
#include "TextureImage.h"

#include "UiUtils.h"

#include "LandscapeContext.h"

using namespace LinearAlg;


/*****************************************************************************
 * Controls data
 ****************************************************************************/

const ivec2 DirectionNW = {-1, 0};
const ivec2 DirectionNE = {0, -1};
const ivec2 DirectionSW = {0, 1};
const ivec2 DirectionSE = {1, 0};

const std::vector<ivec2> ResolutionData = {
    {640, 480},
    {800, 600},
    {1024, 768}
};

const std::vector<std::tuple<std::string, int>> LandscapeSizeData = {
    {"64x64", 6},
    {"128x128", 7},
    {"256x256", 8},
};


/*****************************************************************************
 * LandscapeContext
 ****************************************************************************/

bool LandscapeContext::Init(GLFWwindow* window) {
    LOGI << "OpenGL Renderer  : " << glGetString(GL_RENDERER);
    LOGI << "OpenGL Vendor    : " << glGetString(GL_VENDOR);
    LOGI << "OpenGL Version   : " << glGetString(GL_VERSION);
    LOGI << "GLSL Version     : " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    LOGI << "GLFW Version     : " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION;
    LOGI << "ImGui Version    : " << IMGUI_VERSION << " (" << IMGUI_VERSION_NUM << ")";

    mWindow = window;
    glfwSetWindowUserPointer(mWindow, static_cast<void *>(this));

    glfwGetWindowSize(mWindow, &mWindowWidth, &mWindowHeight);

    map = std::make_unique<VossHeightmap>(landscapeSize);
    image = std::make_unique<TextureImage>(resolution.x, resolution.y);

    minimapImage = std::make_unique<TextureImage>(map->GetWidth(), map->GetWidth());

    map->SetSeekValues(heightSeek, slopeSeek);
    map->Generate(worldPos.x, worldPos.y);

    render.SetSlopeSeek(map->GetSlopeSeek());
    minimapRender.SetSlopeSeek(map->GetSlopeSeek());

    if (!texturedRectangle.Init()) {
        LOGE << "Cannot init textured rectangle";
        return false;
    }
    texturedRectangle.SetResolution(mWindowWidth, mWindowHeight);

    UpdateRenderedLandscape();

    // Buttons Texture
    buttonsTexture.reset(GetButtonTexture());
    if (!buttonsTexture) {
        LOGE << "Cannot create texture object";
        return false;
    }

    return true;
}

void LandscapeContext::UpdateRenderedLandscape() {
    render.Draw(*image, *map);
    image->CopyToTexture();

    minimapRender.Draw(*minimapImage, *map);
    minimapImage->CopyToTexture();
}

void LandscapeContext::Display() {
    glClear(GL_COLOR_BUFFER_BIT); LOGOPENGLERROR();

    image->BindTexture();
    texturedRectangle.Render();

    DisplayUI();
}

void LandscapeContext::DisplayUI() {
    newWorldPos = worldPos;
    newResolutionId = resolutionId;
    newHeightSeek = heightSeek;
    newLandscapeSize = landscapeSize;

    ImGui::Begin("Heightmap Settings");

    ImGui::SeparatorText("Heightmap Size");

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

    ImGui::SeparatorText("Output Resolution");

    for (size_t i = 0; i < ResolutionData.size(); i++) {
        auto res = ResolutionData[i];

        std::stringstream s;
        s << res.x << "x" << res.y;

        if (i > 0) {
            ImGui::SameLine();
        }

        ImGui::RadioButton(s.str().c_str(), &newResolutionId, i);
    }

    ImGui::Separator();

    ImGui::SliderFloat("Height Seek", &newHeightSeek, 1.4f, 3.9f, "%.1f");

    ImGui::SeparatorText("World Navigation");

    ImGui::Text("Position: %d, %d", worldPos.x, worldPos.y);

    if (ImGui::BeginTable("NavigationTable", 3)) {
        ImGui::TableNextRow();

        const ImVec2 NavigationButtonSize = ImVec2(30.f, 30.f);
        const auto buttonsTexId = reinterpret_cast<ImTextureID>(static_cast<GLuint>(buttonsTexture));

        ImGui::TableSetColumnIndex(0);
        if (ImGui::ImageButton("##nw", buttonsTexId, NavigationButtonSize,
            GetButtonUV0(0), GetButtonUV1(0))) {
            newWorldPos += DirectionNW;
        }

        ImGui::TableSetColumnIndex(2);
        if (ImGui::ImageButton("##ne", buttonsTexId, NavigationButtonSize,
            GetButtonUV0(1), GetButtonUV1(1))) {
            newWorldPos += DirectionNE;
        }

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(1);
        {
            ImGui::Dummy(ImVec2(64.f, 128.f));

            ImTextureID my_tex_id = (ImTextureID)(intptr_t)(minimapImage->GetTexture());

            ImVec2 p = ImGui::GetCursorScreenPos();
            ImageRotated(my_tex_id, ImVec2(p.x + 12.f, p.y - 64.f), ImVec2(128.f, 128.f), -M_PI_4);
        }

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        if (ImGui::ImageButton("##sw", buttonsTexId, NavigationButtonSize,
            GetButtonUV0(2), GetButtonUV1(2))) {
            newWorldPos += DirectionSW;
        }

        ImGui::TableSetColumnIndex(2);
        if (ImGui::ImageButton("##se", buttonsTexId, NavigationButtonSize,
            GetButtonUV0(3), GetButtonUV1(3))) {
            newWorldPos += DirectionSE;
        }

        ImGui::EndTable();
    }

    ImGui::SeparatorText("User guide");
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
    texturedRectangle.SetResolution(mWindowWidth, mWindowHeight);
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

    bool landscapeNeedsUpdate = false;

    if (worldPos != newWorldPos || heightSeek != newHeightSeek) {
        worldPos = newWorldPos;
        heightSeek = newHeightSeek;

        LOGI << "Move to " << worldPos.x << "," << worldPos.y;
        LOGI << "Set heightmap parameters to "
            << "heightSeek=" << heightSeek;

        map->SetSeekValues(heightSeek, slopeSeek);

        map->Generate(worldPos.x, worldPos.y);

        image->Clear();

        render.SetSlopeSeek(slopeSeek);
        minimapRender.SetSlopeSeek(slopeSeek);

        landscapeNeedsUpdate = true;
    }

    if (resolutionId != newResolutionId) {
        resolutionId = newResolutionId;
        resolution = ResolutionData[resolutionId];

        LOGI << "Set image resolution to " << resolution.x << "," << resolution.y;

        image = std::make_unique<TextureImage>(resolution.x, resolution.y);

        landscapeNeedsUpdate = true;
    }

    if (landscapeSize != newLandscapeSize) {
        landscapeSize = newLandscapeSize;

        LOGI << "Set landscape size to " << landscapeSize;

        map = std::make_unique<VossHeightmap>(landscapeSize);

        map->SetSeekValues(heightSeek, slopeSeek);
        map->Generate(worldPos.x, worldPos.y);

        image->Clear();

        minimapImage = std::make_unique<TextureImage>(map->GetWidth(), map->GetWidth());

        landscapeNeedsUpdate = true;
    }

    if (landscapeNeedsUpdate) {
        UpdateRenderedLandscape();
    }
}
