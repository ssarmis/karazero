#pragma once

#include "global.hpp"
#include "math.hpp"

struct Vertex {
    v3 p;
    v3 uv;
    v3 n;
    v3 color;
    v3 tangent;

    v4i boneIds;
    v4 boneWeights;

    Vertex() = default;
    Vertex(v3 pos, v3 u, v3 c) :p(pos), uv(u), color(c) {}
    
    Vertex(v3 pos, v3 u, v3 normal, v4i bi, v4 bw) :p(pos), uv(u), n(normal), boneIds(bi), boneWeights(bw) {}

    Vertex(v3 pos, v3 u, v3 normal, v3 c) :p(pos), uv(u), n(normal), color(c) {}
    Vertex(v3 pos, v3 u, v3 normal, v3 c, v3 t) :p(pos), uv(u), n(normal), color(c), tangent(t) {}
};