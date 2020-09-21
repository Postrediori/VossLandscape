#pragma once

class MinimapRenderer : public IHeightmapRenderer {
public:
    MinimapRenderer(VossHeightmap* map, Image* image);

    void draw() override;

    void setImage(Image* image);

private:
    VossHeightmap* m_map;
    Image* m_image;
};
