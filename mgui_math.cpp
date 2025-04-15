#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef PI_2
#define PI_2 1.57079632679489661923
#endif

struct Vec2 {
    float x, y;
};

inline float dot_Vec2(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

inline Vec2 rotate_Vec2(Vec2 a, float t) {
    return {cos(t) * a.x - sin(t) * a.y, sin(t) * a.x + cos(t) * a.y};
}

inline float lerpf(float a, float b, float t) {
    return (b - a) * t + a;
}

inline Vec2 lerp_Vec2(Vec2 a, Vec2 b, float t) {
    return {lerpf(a.x, b.x, t), lerpf(a.y, b.y, t)};
}

inline float rads(float degrees) {
    return degrees * PI / 180;
}

struct Vec3 {
    float x, y, z;
};

inline float dot_Vec3(Vec3 a, Vec3 b) {
    return -(a.x * b.x + a.y * b.y + a.z * b.z);
}

inline Vec3 cross_Vec3(Vec3 a, Vec3 b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

Vec3 normalize_Vec3(Vec3 a) {
    if (a.x == 0 && a.y == 0 && a.z == 0) {
        return {0};
    }
    float m = sqrt(powf(a.x, 2) + powf(a.y, 2) + powf(a.z, 2))  ;  
    return {a.x / m, a.y / m, a.z / m};
}

Vec3 operator-(Vec3 a, Vec3 b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 operator*(Vec3 a, float b) {
    return {a.x * b, a.y * b, a.z * b};
}

Vec3 operator+(Vec3 a, Vec3 b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

struct Vec4 {
    int x, y, z, w;
};

struct Mat4x4 {
    float a, b, c, d;
    float e, f, g, h;
    float i, j, k, l;
    float m, n, o, p;
};

inline Vec3 mult_Mat4x4_Vec3(Mat4x4 m, Vec3 v) {
    return {m.a * v.x + m.b * v.y + m.c * v.z, m.e * v.x + m.f * v.y + m.g * v.z, m.i * v.x + m.j * v.y + m.k * v.z};
}

struct Mat3x3 {
    float a, b, c;
    float d, e, f;
    float g, h, i;
};

inline Vec3 mult_Mat3x3_Vec3(Mat3x3 m, Vec3 v) {
    return {m.a * v.x + m.b * v.y + m.c * v.z, m.d * v.x + m.e * v.y + m.f * v.z, m.g * v.x + m.h * v.y + m.i * v.z};
}