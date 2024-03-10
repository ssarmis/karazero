#pragma once

#include "global.hpp"
#include "math.hpp"

struct Vertex {
    v3 p;
    v2 uv;
    v3 n;
    v3 color;
    v3 tangent;
    Vertex() = default;
    Vertex(v3 pos, v2 u, v3 c) :p(pos), uv(u), color(c) {}
    Vertex(v3 pos, v2 u, v3 normal, v3 c) :p(pos), uv(u), n(normal), color(c) {}
    Vertex(v3 pos, v2 u, v3 normal, v3 c, v3 t) :p(pos), uv(u), n(normal), color(c), tangent(t) {}
};