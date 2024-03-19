#include "global.hpp"
#include "math.hpp"

v3 v3::Lerp(v3 a, v3 b, r32 t) {
    return a * (1.0 - t) + b * t;
}

v4 v4::Lerp(v4 a, v4 b, r32 t) {
    return a * (1.0 - t) + b * t;
}

v4 v4::Slerp(v4 a, v4 b, r32 t) {
    r32 cosTheta = Math::Dot(a, b);
    v4 c = b;
    if (cosTheta < 0) {
        c = -b;
        cosTheta = -cosTheta;
    }

    if (cosTheta > 1 - 0.000001) {
        return v4::Lerp(a, c, t);
    }
    else {
        r32 angle = std::acos(cosTheta);
        r32 invSin = 1 / std::sin(angle);
        r32 c0 = std::sin((1 - t) * angle) * invSin;
        r32 c1 = std::sin(t * angle) * invSin;

        return a * c0 + c * c1;
    }
}

v2 v2::Lerp(v2 a, v2 b, r32 t) {
    return a * (1.0 - t) + b * t;
}

std::ostream& operator<<(std::ostream& stream, v4& v) {
    stream << "V " << std::endl;
    for (int i = 0; i < 4; ++i) {
        stream << v.m[i] << " ";
        stream << std::endl;
    }
    return stream;
}
std::ostream& operator<<(std::ostream& stream, m4& m) {
    stream << "M " << std::endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            stream << m.m[j + i * 4] << " ";
        }
        stream << std::endl;
    }
    return stream;
}

namespace Math {
    r32 Clamp(r32 v, r32 l, r32 h) {
        return v < l ? l : (v > h ? h : v);
    }

    v3 NDCToSC(v3 v, r32 width, r32 height) {
        r32 halfWidth = width / 2.0f;
        r32 halfHeight = height / 2.0f;

        r32 x = (v.x * halfWidth + halfWidth);
        r32 y = (-v.y * halfHeight + halfHeight);

        return v3(x, y, v.z);
    }

    r32 Dot(v4 a, v4 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
    r32 Dot(v3 a, v3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    r32 Dot(v2 a, v2 b) {
        return a.x * b.x + a.y * b.y;
    }
    v3 Intersect(v3 point, v3 normal, v3 a, v3 b, r32* tt) {
        r32 d0 = Dot(normal, (a - point));
        r32 d1 = Dot(normal, (b - point));

        r32 t = d0 / (d0 - d1);
        if (tt) {
            *tt = t;
        }
        return v3::Lerp(a, b, t);
    }

    v4 Intersect(v4 point, v4 normal, v4 a, v4 b, r32* tt) {
        r32 d0 = Dot(normal, (a - point));
        r32 d1 = Dot(normal, (b - point));

        r32 t = d0 / (d0 - d1);
        if (tt) {
            *tt = t;
        }
        return v4::Lerp(a, b, t);
    }

    v3 Reflect(v3 vector, v3 normal) {
        return vector - normal * (2.0f * Dot(vector, normal));
    }
}

v3 v3::Clamp(const v3& v, r32 min, r32 max) {
    v3 result;
    result.x = Math::Clamp(v.x, min, max);
    result.y = Math::Clamp(v.y, min, max);
    result.z = Math::Clamp(v.z, min, max);
    return result;
}