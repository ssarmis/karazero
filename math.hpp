#pragma once

#include "global.hpp"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

enum Axis {
    X,
    Y,
    Z
};

struct v3 {
    union {
        struct {
            r32 x;
            r32 y;
            r32 z;
        };
        r32 m[3];
    };
    v3() = default;
    v3(r32 xx, r32 yy, r32 zz) : x(xx), y(yy), z(zz) {}

    static v3 Cross(v3 a, v3 b) {
        v3 result;
        result.x = a.y * b.z - a.z * b.y;
        result.y = a.z * b.x - a.x * b.z;
        result.z = a.x * b.y - a.y * b.x;
        return result;
    }

    static v3 Perp(v3 v) {
        // z is unused
        return v3(-v.y, v.x, 0);
    }

    static v3 Min(v3 a, v3 b) {
        int minx = a.x < b.x ? a.x : b.x;
        int miny = a.y < b.y ? a.y : b.y;
        int minz = a.z < b.z ? a.z : b.z;
        return v3(minx, miny, minz);
    }
    static v3 Max(v3 a, v3 b) {
        int maxx = a.x > b.x ? a.x : b.x;
        int maxy = a.y > b.y ? a.y : b.y;
        int maxz = a.z > b.z ? a.z : b.z;
        return v3(maxx, maxy, maxz);
    }

    static v3 Clamp(const v3& v, r32 min, r32 max);

    r32 Length() {
        return std::sqrt(x * x + y * y + z * z);
    }

    v3 Normalized() {
        r32 length = Length();
        return v3(x / length, y / length, z / length);
    }

    static v3 Lerp(v3 a, v3 b, r32 t);

    v3 operator-() {
        return v3(-x, -y, -z);
    }

    v3 operator-(v3 b) {
        return v3(x - b.x, y - b.y, z - b.z);
    }
    v3 operator+(v3 b) {
        return v3(x + b.x, y + b.y, z + b.z);
    }
    v3 operator*(v3 b) {
        return v3(x * b.x, y * b.y, z * b.z);
    }
    v3 operator*(r32 b) const {
        return v3(x * b, y * b, z * b);
    }
    v3 operator/(r32 b) {
        return v3(x / b, y / b, z / b);
    }
    v3 operator+(r32 b) {
        return v3(x + b, y + b, z + b);
    }
    v3 operator-(r32 b) {
        return v3(x - b, y - b, z - b);
    }
};

struct v4 {
    union {
        struct {
            r32 x;
            r32 y;
            r32 z;
            r32 w;
        };
        r32 m[4];
    };
    v4() = default;
    v4(r32 xx, r32 yy, r32 zz, r32 ww) : x(xx), y(yy), z(zz), w(ww) {}

    r32 Length() {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    v4 Normalized() {
        r32 length = Length();
        return v4(x / length, y / length, z / length, w / length);
    }

    static v4 Lerp(v4 a, v4 b, r32 t);

    v4 operator/(r32 r) {
        return v4(x / r, y / r, z / r, w / r);
    }

    v4 operator-(v4 b) {
        return v4(x - b.x, y - b.y, z - b.z, w - b.w);
    }
    v4 operator+(v4 b) {
        return v4(x + b.x, y + b.y, z + b.z, w + b.w);
    }
    v4 operator*(v4 b) {
        return v4(x * b.x, y * b.y, z * b.z, w * b.w);
    }
    v4 operator*(r32 b) const {
        return v4(x * b, y * b, z * b, w * b);
    }
};

struct v2 {
    r32 x;
    r32 y;
    v2() = default;
    v2(r32 xx, r32 yy) : x(xx), y(yy) {}

    static v2 Min(v2 a, v2 b) {
        int minx = a.x < b.x ? a.x : b.x;
        int miny = a.y < b.y ? a.y : b.y;
        return v2(minx, miny);
    }
    static v2 Max(v2 a, v2 b) {
        int maxx = a.x > b.x ? a.x : b.x;
        int maxy = a.y > b.y ? a.y : b.y;
        return v2(maxx, maxy);
    }

    r32 Length() {
        return std::sqrt(x * x + y * y);
    }

    v2 Normalized() {
        r32 length = Length();
        return v2(x / length, y / length);
    }

    static v2 Lerp(v2 a, v2 b, r32 t);

    v2 operator-() {
        return v2(-x, -y);
    }
    v2 operator-(v2 b) {
        return v2(x - b.x, y - b.y);
    }
    v2 operator+(v2 b) {
        return v2(x + b.x, y + b.y);
    }
    v2 operator*(v2 b) {
        return v2(x * b.x, y * b.y);
    }
    v2 operator*(r32 b) const {
        return v2(x * b, y * b);
    }
    v2 operator+(r32 b) {
        return v2(x * b, y * b);
    }
};

struct m3 {
    r32 m[3 * 3];

    m3() {
        for (int i = 0; i < 3 * 3; ++i) {
            m[i] = 0;
        }

        m[0 + 0 * 3] = 1;
        m[1 + 1 * 3] = 1;
        m[2 + 2 * 3] = 1;
    }

    static m3 Rotation(r32 angles) {
        r32 rad = angles * (M_PI / 180.0);

        m3 result;

        result.m[0] = std::cos(rad);
        result.m[1] = std::sin(rad);

        result.m[0 + 1 * 3] = -std::sin(rad);
        result.m[1 + 1 * 3] = std::cos(rad);

        return result;
    }

    static m3 Translation(v2 p) {
        m3 result;

        result.m[3] = p.x;
        result.m[3 + 1 * 3] = p.y;

        return result;
    }
};

struct m4 {
    union {
        struct {
            v4 r0;
            v4 r1;
            v4 r2;
            v4 r3;
        };
        struct {
            v4 rows[4];
        };
        r32 m[4 * 4];
    };

    m4() {
        for (int i = 0; i < 4 * 4; ++i) {
            m[i] = 0;
        }

        m[0 + 0 * 4] = 1;
        m[1 + 1 * 4] = 1;
        m[2 + 2 * 4] = 1;
        m[3 + 3 * 4] = 1;
    }

    static m4 Perspective(r32 fov, r32 aspect, r32 near, r32 far) {
        m4 result = {};

#if 1
        r32 fovScale = 1.0f / tan(fov / 2.0f);
        r32 xt = fovScale;
        r32 yt = aspect * fovScale;
        r32 zt = far / (far - near);

        result.rows[0] = v4(xt, 0, 0, 0);
        result.rows[1] = v4(0, yt, 0, 0);
        result.rows[2] = v4(0, 0, zt, -near * zt);
        result.rows[3] = v4(0, 0, 1, 0);
#else
        r32 xt = 1.0;
        r32 yt = aspect;

        result.rows[0] = v4(xt, 0, 0, 0);
        result.rows[1] = v4(0, yt, 0, 0);
        result.rows[2] = v4(0, 0, 1, 0);
        result.rows[3] = v4(0, 0, 1, 0);
#endif
        return result;
    }

    static m4 Rotation(r32 angle, Axis axis) {
        angle = angle * (M_PI / 180.0);

        m4 result;

        switch (axis) {
        case Axis::X: {
            // result.rows[0] = v4(1, 0, 0, 0);
            // result.rows[1] = v4(0, cos(angle), sin(angle), 0);
            // result.rows[2] = v4(0, -sin(angle), cos(angle), 0);
            // result.rows[3] = v4(0, 0, 0, 1);

            result.rows[0] = { 1, 0, 0, 0 };
            result.rows[1] = { 0, cos(angle), -sin(angle), 0 };
            result.rows[2] = { 0, sin(angle), cos(angle), 0 };
            result.rows[3] = { 0, 0, 0, 1 };
            break;
        };

        case Axis::Y: {
            result.rows[0] = { cos(angle), 0, -sin(angle), 0 };
            result.rows[1] = { 0, 1, 0, 0 };
            result.rows[2] = { sin(angle), 0, cos(angle), 0 };
            result.rows[3] = { 0, 0, 0, 1 };
            break;
        };

        case Axis::Z: {
            result.rows[0] = { cos(angle), sin(angle), 0, 0 };
            result.rows[1] = { -sin(angle), cos(angle), 0, 0 };
            result.rows[2] = { 0, 0, 1, 0 };
            result.rows[3] = { 0, 0, 0, 1 };
            break;
        };
        }

        return result;
    }

    static m4 Translation(v3 p) {
        m4 result;

        result.rows[0] = v4(1, 0, 0, p.x);
        result.rows[1] = v4(0, 1, 0, p.y);
        result.rows[2] = v4(0, 0, 1, p.z);
        result.rows[3] = v4(0, 0, 0, 1);

        return result;
    }

    m4 operator*(m4 n) {
        m4 result = {};

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[j + i * 4] = 0;
                for (int k = 0; k < 4; ++k) {
                    result.m[j + i * 4] += m[k + i * 4] * n.m[j + k * 4];
                }
            }
        }
        return result;
    }

    v4 operator*(v3 v) {
        v4 vv(v.x, v.y, v.z, 1);

        v4 result;
        for (int i = 0; i < 4; ++i) {
            result.m[i] = 0;
            for (int j = 0; j < 4; ++j) {
                result.m[i] += m[j + i * 4] * vv.m[j];
            }
        }
        return result;
    }

    v4 operator*(v4 v) {
        v4 result;
        for (int i = 0; i < 4; ++i) {
            result.m[i] = 0;
            for (int j = 0; j < 4; ++j) {
                result.m[i] += m[j + i * 4] * v.m[j];
            }
        }
        return result;
    }

};

namespace Math {
    r32 Clamp(r32 v, r32 l, r32 h);
    v3 NDCToSC(v3 v, r32 width, r32 height);

    r32 Dot(v4 a, v4 b);
    r32 Dot(v3 a, v3 b);
    r32 Dot(v2 a, v2 b);
    v3 Intersect(v3 point, v3 normal, v3 a, v3 b, r32* tt = nullptr);
    v4 Intersect(v4 point, v4 normal, v4 a, v4 b, r32* tt = nullptr);
    v3 Reflect(v3 vector, v3 normal);
}
