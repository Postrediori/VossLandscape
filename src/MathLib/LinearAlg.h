#pragma once

namespace LinearAlg
{
    struct ivec2 {
        int x, y;

        ivec2 operator+(const ivec2& rhs);
        ivec2 operator-(const ivec2& rhs);

        ivec2 operator+=(const ivec2& rhs);
        ivec2 operator-=(const ivec2& rhs);

        bool operator==(const ivec2& rhs);
        bool operator!=(const ivec2& rhs);
    };
}
