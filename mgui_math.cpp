#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef PI_2
#define PI_2 1.57079632679489661923
#endif

class Vec2 {
public:
    float x;
    float y;

    Vec2() = default;
    Vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    inline float magnitude() {
        return sqrt(pow(this->x, 2) + pow(this->y, 2));
    }

    Vec2 operator*(float a) {
        return Vec2(this->x * a, this->y * a);
    }

    Vec2 operator+(Vec2 a) {
        return Vec2(this->x + a.x, this->y + a.y);
    }

    Vec2 operator/(float a) {
        return Vec2(this->x / a, this->y / a);
    }
};

inline float dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

Vec2 rotateVec2(Vec2 a, float t) {
    return Vec2(cos(t) * a.x - sin(t) * a.y, sin(t) * a.x + cos(t) * a.y);
}

inline float lerpf(float a, float b, float t) {
    return (b - a) * t + a;
}

inline Vec2 lerpVec2(Vec2 a, Vec2 b, float t) {
    return Vec2(lerpf(a.x, b.x, t), lerpf(a.y, b.y, t));
}

inline float rads(float degrees) {
    return degrees * PI / 180;
}