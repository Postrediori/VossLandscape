#pragma once

class IHeightmapRenderer {
public:
    virtual ~IHeightmapRenderer() { }
    virtual void draw() = 0;
};
