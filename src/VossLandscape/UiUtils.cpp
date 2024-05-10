#include "pch.h"
#include "GraphicsLogger.h"
#include "UiUtils.h"


const uint8_t ButtonsTexture[ButtonsTextureWidth * ButtonsTextureHeight + 1] =
    "        "
    " XX  XX "
    " X    X "
    "        "
    "        "
    " X    X "
    " XX  XX "
    "        ";

GLuint GetButtonTexture() {
    GLuint tex;

    glGenTextures(1, &tex); LOGOPENGLERROR();
    if (!tex) {
        LOGE << "Cannot create texture object";
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, tex); LOGOPENGLERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); LOGOPENGLERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); LOGOPENGLERROR();

    std::vector<uint32_t> data(sizeof(ButtonsTexture));

    for (size_t i = 0; i < sizeof(ButtonsTexture); i++) {
        data[i] = [](uint8_t c) -> uint32_t {
            switch (c) {
            case 'X': return 0xFFFFFFFF;
            case ' ': return 0x00000000;
            default: return 0x00000000;
            }
        }(ButtonsTexture[i]);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ButtonsTextureWidth, ButtonsTextureHeight, 0, GL_RGBA,
             GL_UNSIGNED_BYTE, data.data()); LOGOPENGLERROR();

    return tex;
}

void ImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    ImVec2 pos[4] =
    {
        center + ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
        center + ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
        center + ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a),
        center + ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a)
    };
    ImVec2 uvs[4] = 
    {
        ImVec2(0.0f, 0.0f),
        ImVec2(1.0f, 0.0f),
        ImVec2(1.0f, 1.0f),
        ImVec2(0.0f, 1.0f)
    };

    draw_list->AddImageQuad(tex_id,
        pos[0], pos[1], pos[2], pos[3],
        uvs[0], uvs[1], uvs[2], uvs[3],
        IM_COL32_WHITE);
}
