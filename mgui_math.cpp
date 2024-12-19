class Vec2 {
public:
    float x;
    float y;

    Vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }
};

inline float lerpf(float a, float b, float t) {
    return (b - a) * t + a;
}

inline Vec2 lerpVec2(Vec2 a, Vec2 b, float t) {
    return Vec2(lerpf(a.x, b.x, t), lerpf(a.y, b.y, t));
}

float powf(float x, int y) {
    float temp;
    if (y == 0) {
        return 1;
    }

    temp = powf(x, y / 2);
    if ((y % 2) == 0) {
        return temp * temp;
    } else {
        if (y > 0) {
            return x * temp * temp;
        } else {
            return (temp * temp) / x;
        }
    }
}