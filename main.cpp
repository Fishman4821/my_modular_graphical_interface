#include <iostream>
#include <vector>

#define SDL_MAIN_HANDLED
#define FPS 60.0f

#include "mgui.cpp"

using namespace std;

int main() {
    State state = State("cubic bezier test", 640, 480);

    state.r.rect(120, 40, 520, 440, Color(255, 255, 255));

    Vec2 p0 = Vec2(0, 0);
    Vec2 p1 = Vec2(200, 0);
    Vec2 p2 = Vec2(200, 400);
    Vec2 p3 = Vec2(400, 400);

    p0.x += 120;
    p0.y = 400 - p0.y + 40;
    
    p1.x += 120;
    p1.y = 400 - p1.y + 40;
    
    p2.x += 120;
    p2.y = 400 - p2.y + 40;
    
    p3.x += 120;
    p3.y = 400 - p3.y + 40;

    state.r.cubic_bezier(p0, p1, p2, p3, 4, Color(255, 0, 0));

    while (!state.quit) {
        state.update();
    }
}