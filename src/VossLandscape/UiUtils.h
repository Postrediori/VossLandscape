#pragma once

// Buttons
constexpr size_t ButtonsTextureWidth = 8;
constexpr size_t ButtonsTextureHeight = 8;

inline ImVec2 GetButtonUV0(int i) { return ImVec2(static_cast<float>(i % 2) * 0.5f, static_cast<float>(i / 2) * 0.5f); }
inline ImVec2 GetButtonUV1(int i) { return ImVec2(static_cast<float>(i % 2 + 1) * 0.5f, static_cast<float>(i / 2 + 1) * 0.5f); }

GLuint GetButtonTexture();

// Rotated image
inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a) {
    return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
}

void ImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle);
