#include "stdafx.h"

#include "GlUtils.h"
#include "GlShader.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "LogFormatter.h"
#include "SdlContext.h"

#include "Math.h"
#include "Image.h"
#include "GlImage.h"
#include "VossHeightmap.h"
#include "HeightmapRender.h"

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
 * Main
 ****************************************************************************/
int main(int /*argc*/, char** /*argv*/)
{
    static plog::ConsoleAppender<plog::LogFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    SdlContext sdlContext(g_Width, g_Height, g_Title);
    if (!sdlContext.Init()) {
        LOGE << "Failed to initialize SDL context";
        return -1;
    }

    // Init OpenGL objects
    ShaderProgram program;
    VertexBuffer pointsVbo(GL_TRIANGLES, g_VerticesCount, g_Vertices);
    VertexArray pointsVao(program);

    // Init Heightmap objects
    int landscapeSize = g_LandscapeSize;
    Vec2 resolution = {g_Width, g_Height};
    Vec2 worldPos = {10, 10};
    double heightSeek = 2.0, slopeSeek = 2200.0;

    std::unique_ptr<VossHeightmap> map(new VossHeightmap(landscapeSize));
    std::unique_ptr<GlImage> image(new GlImage(resolution.x, resolution.y));
    std::unique_ptr<HeightmapRender> render(new HeightmapRender(map.get(), image.get()));

    map->SetSeekValues(heightSeek, slopeSeek);
    map->generate(worldPos.x, worldPos.y);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(sdlContext.GetWindow(), sdlContext.GetGlContext());
    ImGui_ImplOpenGL3_Init(sdlContext.GetGlslVersion().c_str());

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done) {
        Vec2 newWorldPos = worldPos;
        Vec2 newResolution = resolution;
        static float newHeightSeek = 2.0f;
        static float newSlopeSeek = 2200.0f;
        int newLandscapeSize = landscapeSize;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    done = true;
                }
                break;

            case SDL_QUIT:
                done = true;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.windowID == SDL_GetWindowID(sdlContext.GetWindow())) {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        done = true;
                    }
                }
                break;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(sdlContext.GetWindow());
        ImGui::NewFrame();

        do {
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
                newResolution = {640, 480};
            }
            ImGui::SameLine();
            if (ImGui::Button("800x600")) {
                newResolution = {800, 600};
            }
            ImGui::SameLine();
            if (ImGui::Button("1024x768")) {
                newResolution = {1024, 768};
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

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)"
                , 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        } while(0);

        // Check status change
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

        // Rendering
        ImGui::Render();

        sdlContext.MakeCurrent();
        
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        do {
            program.UseProgram();

            image->BindTexture();

            program.SetUniforms((int)io.DisplaySize.x, (int)io.DisplaySize.y);

            pointsVbo.BindBuffer();

            pointsVao.BindVertexArray();

            render->draw();
            image->CopyToTexture();

            pointsVbo.DrawArrays();

            pointsVao.UnbindVertexArray();
        } while (0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        sdlContext.GlSwapWindow();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
