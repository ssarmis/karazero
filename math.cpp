#include "global.hpp"
#include "math.hpp"

v3 v3::Lerp(v3 a, v3 b, r32 t) {
    return a * (1.0 - t) + b * t;
}

v4 v4::Lerp(v4 a, v4 b, r32 t) {
    return a * (1.0 - t) + b * t;
}

v2 v2::Lerp(v2 a, v2 b, r32 t) {
    return a * (1.0 - t) + b * t;
}

m3 operator*(m3 m, m3 n) {
    m3 result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.m[j + i * 3] = 0;
            for (int k = 0; k < 3; ++k) {
                result.m[j + i * 3] += m.m[k + i * 3] * n.m[i + k * 3];
            }
        }
    }
    return result;
}

v3 operator*(m3 m, v3 v) {
    v3 result;
    for (int i = 0; i < 3; ++i) {
        result.m[i] = 0;
        for (int j = 0; j < 3; ++j) {
            result.m[i] += m.m[i + j * 3] * v.m[j];
        }
    }
    return result;
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

v2 operator*(m3 m, v2 v) {
    v3 result = m * v3(v.x, v.y, 0);
    return v2(result.x, result.y);
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