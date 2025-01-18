#include <iostream>
#include <vector>

#define SDL_MAIN_HANDLED
#define FPS 60.0f

#include "mgui.cpp"

using namespace std;

class Transform {
public:
    Vec2 pos;
    float rot;

    Transform() = default;
    Transform(Vec2 pos, float rot) {
        this->pos = pos;
        this->rot = rot;
    }
};

class Collider {
public:
    Vec2* points;
    int num_points;

    void destroy() {
        free(this->points);
    }
};

bool ccw(Vec2 a, Vec2 b, Vec2 c) {
    return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
}

bool will_collide_face(Vec2 pos, Vec2 vel, Vec2 a, Vec2 b, float dt) {
    Vec2 c = pos + (vel * dt);
    return ccw(pos, a, b) != ccw(c, a, b) and ccw(pos, c, a) != ccw(pos, c, b);
}

inline Vec2 draw_space(Vec2 a) {
    return Vec2(a.x + 320, 480 - (a.y + 240));
}

Vec2 collision(State* state, Collider* a, Vec2 a_pos, Vec2 vel, Collider* b, Vec2 b_pos, float dt) {
    bool will_collide = false;
    int index_a, index_b;
    for (int i = 0; i < a->num_points; i++) {
        for (int j = 0; j < b->num_points - 1; j++) {
            state->r.point(draw_space(a_pos + a->points[i]), Color(0, 255, 0));
            state->r.point(draw_space(a_pos + a->points[i] + vel * dt), Color(0, 255, 255));
            state->r.point(draw_space(b_pos + b->points[j] * 1.01), Color(0, 255, 0));
            state->r.line(draw_space(b_pos + b->points[j]), draw_space(b_pos + b->points[j + 1]), Color(255, 0, 0));
            if (will_collide_face(a_pos + a->points[i], vel, b_pos + b->points[j], b_pos + b->points[j + 1], dt)) {
                will_collide = true;
                index_a = i;
                index_b = j;
                break;
            }
        }
        if (will_collide) {
            break;
        }
    }
    if (!will_collide) {
        return vel;
    }

    Vec2 b1 = b->points[index_b];
    Vec2 b2 = b->points[index_b + 1];
    Vec2 b_normal_unit = Vec2(-(b2.x - b1.x), b2.y - b1.y) / Vec2(-(b2.x - b1.x), b2.y - b1.y).magnitude();
    state->r.line(draw_space(Vec2(0, 0)), draw_space(b_normal_unit * 25), Color(255, 0, 255));
    float b_angle_to_ref = acos(b_normal_unit.y);
    state->r.point(draw_space(Vec2(sin(b_angle_to_ref) * 30, cos(b_angle_to_ref) * 30)), Color(255, 255, 0));
    state->r.point(draw_space(Vec2(sin(2.0 * PI - b_angle_to_ref) * 30, cos(2.0 * PI - b_angle_to_ref) * 30)), Color(0, 255, 0));
    Vec2 vel_rotated = rotateVec2(vel, 2.0 * PI - b_angle_to_ref);
    state->r.line(draw_space(Vec2(0, 0)), draw_space(vel * 20), Color(0, 0, 255));
    state->r.line(draw_space(Vec2(0, 0)), draw_space(vel_rotated * 20), Color(0, 255, 255));
    state->r.line(draw_space(a_pos), draw_space(a_pos + vel * 5), Color(0, 0, 255));
    state->r.line(draw_space(a_pos), draw_space(a_pos + vel_rotated * 5), Color(0, 255, 255));

    return vel_rotated;
}

void ball_init(Element* e, State* state) {
    Transform* t = (Transform*)(e->data.value(0));
    *t = Transform(Vec2(-175, 0), 0);
    Vec2* vel = (Vec2*)(e->data.value(2));
    *vel = Vec2(0, 0);
}

void ball_update(Element* e, State* state) {
    Transform* t = (Transform*)(e->data.value(0));
    Collider* ca = *(Collider**)(e->data.value(1));
    Collider* cb = *(Collider**)(e->data.value(3));
    Vec2* vel = (Vec2*)(e->data.value(2));
    vel->x = -10;

    *vel = collision(state, ca, t->pos, *vel, cb, Vec2(0, 0), state->t.dt);
    
    vel->y -= 0.1;

    t->pos.x += vel->x * state->t.dt;
    t->pos.y += vel->y * state->t.dt;

    // for (int i = 0; i < c->num_points; i++) {
    //     state->r.point(c->points[i].x + t->pos.x + 320, 480 - (c->points[i].y + t->pos.y + 240), Color(0, i * 200 / c->num_points + 55, 0));
    // }
}

int main() {
    State state = State("ball collisions", 640, 480);

    const int num_points1 = 36;
    Collider c1;
    c1.points = (Vec2*)malloc(sizeof(Vec2) * num_points1);
    c1.num_points = num_points1;
    float a = 0.0f;
    for (int i = 0; i < num_points1; i++) {
        a = i * 360 / num_points1;
        c1.points[i] = Vec2(sin(rads(a)) * 15, cos(rads(a)) * 15);
    }

    const int num_points2 = 180;
    Collider c2;
    c2.points = (Vec2*)malloc(sizeof(Vec2) * num_points2);
    c2.num_points = num_points2;
    for (int i = 0; i < num_points2; i++) {
        a = i * 360 / num_points2;
        c2.points[i] = Vec2(sin(rads(a)) * 200, cos(rads(a)) * 200);
    }

    size_t ball_sizes[4] = {sizeof(Transform), sizeof(Collider*), sizeof(Vec2), sizeof(Collider*)};

    Element ball = Element("ball1", ball_sizes, 4, ball_update, ball_init, true);
    Collider** ball_ca = (Collider**)(ball.data.value(1));
    *ball_ca = &c1;
    Collider** ball_cb = (Collider**)(ball.data.value(3));
    *ball_cb = &c2;
    
    state.add_element(&ball);

    while (!state.quit) {
        state.r.rect(0, 0, 640, 480, Color(0, 0, 0));
        state.r.point(draw_space(Vec2(0, 0)), Color(255, 0, 0));
        state.update();
    }

    ball.destroy();
    c1.destroy();
    c2.destroy();
}