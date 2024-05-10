#include "pch.h"
#include "LinearAlg.h"

namespace LinearAlg
{
    ivec2 ivec2::operator+(const ivec2& rhs) {
        return {x + rhs.x, y + rhs.y};
    }
    ivec2 ivec2::operator-(const ivec2& rhs) {
        return {x - rhs.x, y - rhs.y};
    }

    ivec2 ivec2::operator+=(const ivec2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    ivec2 ivec2::operator-=(const ivec2& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    bool ivec2::operator==(const ivec2& rhs) {
        return (x == rhs.x) && (y == rhs.y);
    }
    bool ivec2::operator!=(const ivec2& rhs) {
        return !(*this == rhs);
    }
}
