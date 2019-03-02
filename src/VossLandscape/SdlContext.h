#pragma once

class SdlContext
{
public:
    SdlContext(int width, int height, const std::string& title);
    ~SdlContext();

    int Init();

    void GlSwapWindow();
    void MakeCurrent();

    SDL_Window* GetWindow() const { return m_window; }
    SDL_GLContext GetGlContext() const { return m_glContext; }
    std::string GetGlslVersion() const { return m_glslVersion; }

private:
    int m_width, m_height;
    std::string m_title;

    SDL_WindowFlags m_windowFlags;

    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    std::string m_glslVersion;
};
