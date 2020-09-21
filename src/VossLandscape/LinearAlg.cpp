#include "stdafx.h"
#include "LinearAlg.h"

bool linearalg::operator!=(const linearalg::ivec2& lhs, const linearalg::ivec2& rhs) {
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}
