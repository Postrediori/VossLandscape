#pragma once

#include "BaseHeightmapRenderer.h"

class HeightmapRender : public BaseHeightmapRenderer {
public:
    void Draw(BaseImage& image, BaseHeightmap& heightmap) override;

private:
    void ClearC();
    void DrawHoriz();
    void DrawVert();
    void p5000();
    void p6000();
    void p6500(int tick);
    void p7000();
    void p7500(int tick);
    void p8000();
    void p8100();
    void p8200();

private:
    BaseImage* m_pImage;
    BaseHeightmap* m_pHeightmap;

    std::vector<uint16_t> m_c;

    double m_z;
    double m_zScale, m_scale;
    double m_x, m_y;
    double m_xt, m_yt;
    double m_xi, m_yi;
    double m_dx, m_dy;
    double m_xh, m_yh;
    double m_xmin, m_ymin;
    double m_xmax, m_ymax;
    double m_xold, m_yold;
};
