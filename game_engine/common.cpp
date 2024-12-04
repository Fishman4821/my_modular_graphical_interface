#pragma once

#include "SDL.h"
#include <math.h>
#include <iostream>

#include "obj.cpp"

using namespace std;

const char* windowTitle = "elite";
bool quit = false;

class color {
public:
    uint8_t r, g, b, a;
    color() = default;
    color(uint8_t r_, uint8_t g_, uint8_t b_) {
        r = r_;
        g = g_;
        b = b_;
        a = 255;
    }
    color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_) {
        r = r_;
        g = g_;
        b = b_;
        a = a_;
    }
    int bytes() {
        return r << 12 | g << 8 | b << 4 | a;
    }
};

class Vec3 {
public:
    float x, y, z;
    Vec3() = default;
    Vec3(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vec3 rotateX(Vec3 pos, float t) {
	    return Vec3(pos.x, (float)(pos.y * cos(t) + pos.z * sin(t)), (float)(pos.z * cos(t) - pos.y * sin(t)));
    }

    Vec3 rotateY(Vec3 pos, float t) {
	    return Vec3((float)(pos.x * cos(t) - pos.z * sin(t)), pos.y, (float)(pos.x * sin(t) + pos.z * cos(t)));
    }

    Vec3 rotateZ(Vec3 pos, float t) {
	    return Vec3((float)(pos.x * cos(t) + pos.y * sin(t)), (float)(pos.y * cos(t) - pos.x * sin(t)), pos.z);
    }

    Vec3 rotate(Vec3 rotation) {
        Vec3 val = Vec3(
            x,//cos(rotation.x) * cos(rotation.y) * x + (cos(rotation.x) * sin(rotation.y) * sin(rotation.z) - sin(rotation.x) * cos(rotation.z)) * y + (sin(rotation.x) * sin(rotation.z) + cos(rotation.x) * sin(rotation.y) * cos(rotation.z)) * z,
            y,//sin(rotation.x) * cos(rotation.y) * x + (cos(rotation.x) * cos(rotation.z) * sin(rotation.x) - sin(rotation.y) * sin(rotation.z)) * y + (sin(rotation.x) * sin(rotation.z) * cos(rotation.x) - cos(rotation.x) * sin(rotation.z)) * z,
            z//-x * sin(rotation.y) + y * cos(rotation.y) + z * cos(rotation.y) * cos(rotation.z)
        );
        val = rotateX(val, rotation.x);
        val = rotateY(val, rotation.y);
        val = rotateZ(val, rotation.z);
        return val;
    }

    Vec3 operator+(Vec3 a) {
        return Vec3(x + a.x, y + a.y, z + a.z);
    }

    Vec3 operator*(Vec3 a) {
        return Vec3(x * a.x, y * a.y, z * a.z);
    }

    Vec3 operator*(float a) {
        return Vec3(x * a, y * a, z * a);
    }

    Vec3 operator-(Vec3 a) {
        return Vec3(x - a.x, y - a.y, z - a.z); 
    }

    Vec3 operator/(float a) {
        return Vec3(x / a, y / a, z / a);
    }

    float distance_from(Vec3 p) {
        return sqrtf(powf(p.x - x, 2) + powf(p.y - y, 2) + powf(p.z - z, 2));
    }

    float dot(Vec3 a) {
        return x * a.x + y * a.y + z * a.z;
    }

    Vec3 cross(Vec3 a) {
        return Vec3(y * a.z - z * a.y, z * a.x - x * a.y, x * a.y - y * a.x);
    }

    Vec3 negate() {
        return Vec3(-x, -y, -z);
    }

    float magnitude() {
        return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }

    Vec3 normalize() {
        float magnitude = this->magnitude();
        return Vec3(x, y, z) / magnitude;
    }
    
    void operator+=(Vec3 a) {
        x += a.x;
        y += a.y;
        z += a.z;
    }
};

class Vec3_Int {
public:
    int x, y, z;
    Vec3_Int(int _x, int _y, int _z) {
        x = _x;
        y = _y;
        z = _z;
    }
};

class Vec2 {
public:
    float x, y;
    Vec2(float _x, float _y) {
        x = _x;
        y = _y;
    }
};

class Transform {
public:
	Vec3 position;
    Vec3 rotation;
    Vec3 scale;

    Transform() {
        position = Vec3(0, 0, 0);
        rotation = Vec3(0, 0, 0);
        scale = Vec3(1, 1, 1);
    }
};

class Mesh {
public:
    Vec3* verts;
    int vert_count;
    Vec3_Int* v_map;
    int v_map_length;
    Vec2* uv_map;
    SDL_Texture* t;
    bool uv_enable;

    Mesh(char* file_path) {
        // FILE* fp = fopen(file_path, "r");
        // if (fp == NULL) {
        //     printf("ERROR: could not open file, exiting.");
        //     exit(1);
        // }
        // char* buffer = (char*)malloc(sizeof(char) * 128);
        // int buffer_index = 0;
        // char current = 0;
        // int vs = 0;
        // int fs = 0;
        // while (true) {
        //     current = getc(fp);
        //     if (current == EOF) {
        //         break;
        //     }
        //     buffer[buffer_index] == current;
        //     buffer_index++;
        //     if (buffer[buffer_index - 1] == 'v' && buffer[buffer_index] == ' ') {
        //         vs++;
        //     }
        //     if (buffer[buffer_index - 1] == 'f' && buffer[buffer_index] == ' ') {
        //         fs++;
        //     }
        //     if (current == '\n') {
        //         memset(buffer, 0, sizeof(char) * 128);
        //         buffer_index = 0;
        //     }
        //     printf("%i\t%c", current, current);
        //     printf("\n");
        // }
        // printf("%f, %f", vs, fs);
        // free(buffer);
        load_object_file(file_path);
    }

    Mesh(Vec3* _verts, int _vert_count, Vec3_Int* _v_map, int _v_map_length) {
        vert_count = _vert_count;
        verts = (Vec3*)malloc(sizeof(Vec3) * _vert_count);
        memcpy(verts, _verts, sizeof(Vec3) * _vert_count);
        
        v_map_length = _v_map_length;
        v_map = (Vec3_Int*)malloc(sizeof(Vec3_Int) * _v_map_length);
        memcpy(v_map, _v_map, sizeof(Vec3_Int) * _v_map_length);

        uv_enable = false;
        t = NULL;
    }

    Mesh(Vec3* _verts, int _vert_count, Vec3_Int* _v_map, int _v_map_length, Vec2* _uv_map, SDL_Texture* _t) {
        vert_count = _vert_count;
        verts = (Vec3*)malloc(sizeof(Vec3) * _vert_count);
        memcpy(verts, _verts, sizeof(Vec3) * _vert_count);
        
        v_map_length = _v_map_length;
        v_map = (Vec3_Int*)malloc(sizeof(Vec3_Int) * _v_map_length);
        memcpy(v_map, _v_map, sizeof(Vec3_Int) * _v_map_length);

        uv_enable = true;
        uv_map = (Vec2*)malloc(sizeof(Vec2) * _vert_count);
        memcpy(uv_map, _uv_map, sizeof(Vec2) * _vert_count);
        t = _t;
    }

    void destruct() {
        free(verts);
        free(v_map);
        free(uv_map);
        free(t);
    }
};

class Camera {
public:
    Transform t;
    float n; // near plane
    float f; // far plane
    float fov;
    float r_project; // constant
    float t_project; // constant
    Vec2 resolution;
    int prev_x = 0;
    int prev_y = 0;

    void set_fov(float _fov) {
        fov = _fov;
        t_project = tan(fov / 2) * n;
        r_project = t_project * resolution.x / resolution.y;
    }

    void updateCamera() {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        int x, y = 0;
        const Uint32 mouse = SDL_GetMouseState(&x, &y);
        float amount_forward = 0;
        float amount_backward = 0;
        float amount_right = 0;
        float amount_left = 0;
        float amount_up = 0;
        float amount_down = 0;
        float speed = 2.5;

        if (keys[SDL_SCANCODE_W]) {
            amount_forward = 1.0;
        }
        if (keys[SDL_SCANCODE_S]) {
            amount_backward = 1.0;
        }
        if (keys[SDL_SCANCODE_A]) {
            amount_right = 1.0;
        }
        if (keys[SDL_SCANCODE_D]) {
            amount_left = 1.0;
        }
        if (keys[SDL_SCANCODE_Q]) {
            amount_down = 1.0;
        }
        if (keys[SDL_SCANCODE_E]) {
            amount_up = 1.0;
        }

        float yaw_cos = cos(t.rotation.y);
        float yaw_sin = sin(t.rotation.y);
        Vec3 forward = Vec3(yaw_sin, 0.0, yaw_cos).normalize();
        Vec3 right = Vec3(yaw_cos, 0.0, -yaw_sin).normalize();
        t.position += forward * (amount_forward - amount_backward) * speed;
        t.position += right * (amount_right - amount_left) * speed;

        t.position.y += (amount_up - amount_down) * speed;

        if (mouse & SDL_BUTTON(3)) {
            t.rotation.y -= (prev_x - x) * 0.01;
            t.rotation.x += (prev_y - y) * 0.01;
        }

        prev_x = x;
        prev_y = y;
        //printf("Pos (%f, %f, %f)\t Rot (%f, %f, %f)\n", t.position.x, t.position.y, t.position.z, t.rotation.x, t.rotation.y, t.rotation.z);
    }
};

class Vec4 {
public:
    float x, y, z, w;

    Vec4(float _x, float _y, float _z, float _w) {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }
};