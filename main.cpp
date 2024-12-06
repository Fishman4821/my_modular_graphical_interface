#include <iostream>
#include <vector>
#include <math.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_ttf.h"

using namespace std;

class Color {
public:
    char r, g, b, a;

    Color(char r, char g, char b, char a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color(char r, char g, char b) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = 255;
    }
};

class Data {
public:
    size_t* sizes;
    void* data;
    int len;

    Data() = default;
    Data(void* data, size_t* sizes) {
        this->data = data;
        this->sizes = sizes;
    }

    Data(void* data, size_t* sizes, int len) {
        this->data = data;
        this->sizes = sizes;
        this->len = len;
    }

    Data(size_t* sizes, int len) {
        size_t total = 0;
        for (int i = 0; i < len; i++) {
            total += sizes[i];
        }

        this->data = malloc(total);
        this->sizes = sizes;
        this->len = len;
    }

    void free_data() {
        free(data);
    }

    void* value(int index) {
        if (this->data == nullptr) {
            return nullptr;
        }

        int offset = 0;
        for (int i = 0; i < index; i++) {
            offset += this->sizes[i];
        }
        return this->data + offset;
    }
};

class State;

class Element {
public:
    const char* name;
    void (*update_fn)(Element*, State*);
    void (*init_fn)(Element*, State*);
    Data data;

    Element() = default;
    Element(const char* name, void (*update_fn)(Element*, State*)) {
        this->name = name;
        this->data = Data(nullptr, nullptr);
        this->update_fn = update_fn;
        this->init_fn = nullptr;
    }

    Element(const char* name, size_t* sizes, int num_sizes, void (*update_fn)(Element*, State*)) {
        this->name = name;
        this->data = Data(sizes, num_sizes);
        this->update_fn = update_fn;
        this->init_fn = nullptr;
    }

    Element(const char* name, void (*update_fn)(Element*, State*), void (*init_fn)(Element*, State*)) {
        this->name = name;
        this->data = Data(nullptr, nullptr);
        this->update_fn = update_fn;
        this->init_fn = init_fn;
    }

    Element(const char* name, size_t* sizes, int num_sizes, void (*update_fn)(Element*, State*), void (*init_fn)(Element*, State*)) {
        this->name = name;
        this->data = Data(sizes, num_sizes);
        this->update_fn = update_fn;
        this->init_fn = init_fn;
    }

    void update(State* state) {
        (update_fn)(this, state);
    }

    void init(State* state) {
        (init_fn)(this, state);
    }

    void destroy() {
        this->data.free_data();
    }
};

class Renderer {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    
public:
    Renderer() = default;
    Renderer(const char* windowTitle, int width, int height) {
        SDL_Init(SDL_INIT_VIDEO);
        this->window = SDL_CreateWindow(
            windowTitle,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            0);

        this->renderer = SDL_CreateRenderer(window, -1, 0);
    }

    void rect(int x1, int y1, int x2, int y2, Color fill) {
        SDL_SetRenderDrawColor(this->renderer, fill.r, fill.g, fill.b, fill.a);
        SDL_Rect rect;
        rect.x = x1;
        rect.y = y1;
        rect.w = x2 - x1; 
        rect.h = y2 - y1;
        SDL_RenderFillRect(this->renderer, &rect);
    }

    void triangle(int x1, int y1, int x2, int y2, int x3, int y3, Color fill) {
        SDL_Vertex vertices[] = {
        {{x1, y1}, {fill.r, fill.g, fill.b, fill.a}, {0, 0}},
        {{x2, y2}, {fill.r, fill.g, fill.b, fill.a}, {0, 0}},
        {{x3, y3}, {fill.r, fill.g, fill.b, fill.a}, {0, 0}}};
        SDL_RenderGeometry(this->renderer, NULL, vertices, 3, NULL, 0);
    }

    void text(const char* text, int x, int y, int w, int h, Color fill) {
        
    }

    void draw() {
        SDL_RenderPresent(this->renderer);
    }
};

class Inputs {
    public:
    struct {
        const Uint8* keys;
    } keyboard;

    struct {
        int x, y;
        bool buttons[3];
    } mouse;

    void update_input() {
        this->keyboard.keys = SDL_GetKeyboardState(NULL);

        Uint32 mouse_state = SDL_GetMouseState(&this->mouse.x, &this->mouse.y);
        this->mouse.buttons[0] = mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT);
        this->mouse.buttons[1] = mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
        this->mouse.buttons[2] = mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT);
    }
};

class Time {
public:
    float delta_time;

    Time() {
        this->delta_time = 0.05;
    }
};

class State {
private:
    vector<Element*> update_list;
    vector<Element*> init_list;

public:
    bool quit;

    Renderer r;
    Inputs i;
    Time t;

    State(const char* windowTitle, int width, int height) {
        this->quit = false;
        this->r = Renderer(windowTitle, width, height);
        this->t = Time();
        this->update_list = {};
    }

    void add_element(Element* e) {
        this->update_list.push_back(e);
        if (e->init_fn != nullptr) {
            this->init_list.push_back(e);
        }
    }

    void remove_element(char name[32]) {
        for (int i = 0; i < this->update_list.size(); i++) {
            if (this->update_list.at(i)->name == name) {
                this->update_list.erase(this->update_list.begin() + i);
            }
        }
    }

    Element* get_element(char name[32]) {
        for (int i = 0; i < this->update_list.size(); i++) {
            if (this->update_list.at(i)->name == name) {
                return this->update_list.at(i);
            }
        }
        return nullptr;
    }

    void update() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                this->quit = true;
            } 
            break;
        }

        i.update_input();

        for (int i = 0; i < this->init_list.size(); i++) {
            this->init_list[i]->init(this);
        }
        init_list.clear();
        for (int i = 0; i < this->update_list.size(); i++) {
            this->update_list[i]->update(this);
        }
        
        r.draw();
    }
};

void update_background(Element* e, State* state) {
    state->r.rect(0, 0, 640, 480, Color(69, 69, 69));
}

void update_player1(Element* e, State* state) {
    float* y = (float*)(e->data.value(0));

    state->r.rect(20, *y + 30, 30, *y - 30, Color(40, 40, 40));

    if (state->i.keyboard.keys[SDL_SCANCODE_W] && *y > 40) {
        *y -= 0.1;
    }
    if (state->i.keyboard.keys[SDL_SCANCODE_S] && *y < 440) {
        *y += 0.1;
    }
}

void update_player2(Element* e, State* state) {
    float* y = (float*)(e->data.value(0));
    state->r.rect(610, *y + 30, 620, *y - 30, Color(40, 40, 40));

    if (state->i.keyboard.keys[SDL_SCANCODE_UP] && *y > 40) {
        *y -= 0.1;
    }
    if (state->i.keyboard.keys[SDL_SCANCODE_DOWN] && *y < 440) {
        *y += 0.1;
    }
}

void init_ball(Element* e, State* state) {
    Element* player1 = state->get_element("player1"); // get pointer to the element for player1
    Element* player2 = state->get_element("player2");
    float** ball_player1_y = (float**)e->data.value(0); // get the ball's pointer to a float* (pointer to the pointer of the y value of player1)
    float** ball_player2_y = (float**)e->data.value(1);
    *ball_player1_y = (float*)player1->data.value(0); // set the value at the ball's pointer to the float* that points to the y value of player 1
    *ball_player2_y = (float*)player2->data.value(0);
    float* x_pos = ((float*)e->data.value(2));
    float* y_pos = ((float*)e->data.value(3));
    *x_pos = 320;
    *y_pos = 240;
    float* total_vel = (float*)(e->data.value(4));
    float* split_vel = (float*)(e->data.value(5));
    *total_vel = 1;
    *split_vel = -0.75;
}

void update_ball(Element* e, State* state) {
    float* player1_y = *(float**)(e->data.value(0)); // get the pointer to the ball's pointer to the value of player1's y then get the value from that pointer (contains pointer to player1_y)
    float* player2_y = *(float**)(e->data.value(1));
    float* x_pos = ((float*)e->data.value(2));
    float* y_pos = ((float*)e->data.value(3));
    float* total_vel = (float*)(e->data.value(4));
    float* split_vel = (float*)(e->data.value(5));
    //*y_pos = *player1_y; // set the ball y value to player1's y value
    //cout << "player1: {f: " << *player1_y << ", i: " << (unsigned long long*)(player1_y) << ", *f: " << **(float**)player1_y<< "}\t";
    //cout  << *player2_y << "\t" << *x_pos << "\t" << *y_pos << "\n";
    //cout << *x_pos << "\t" << *y_pos << "\t" << (*x_pos + 10 >= 610) << "\t" << !(*player2_y + 30 < *y_pos - 10) << "\t" << !(*player2_y - 30 > *y_pos + 10) << "\n";
    
    if ((*x_pos - 10 <= 30 && !(*player1_y + 30 < *y_pos - 10) && !(*player1_y - 30 > *y_pos + 10)) || 
        (*x_pos + 10 >= 610 && !(*player2_y + 30 < *y_pos - 10) && !(*player2_y - 30 > *y_pos + 10))) {
        *split_vel = (1 - *split_vel) * -1;
    }

    *x_pos += *total_vel * *split_vel * state->t.delta_time;
    *y_pos += *total_vel * (1 - abs(*split_vel)) * signbit(*split_vel) * state->t.delta_time;
    cout << *total_vel << "\t" << *split_vel << "\t" << (1 - *split_vel) * -1 << "\t" << *total_vel * *split_vel << "\t" << *total_vel * (1 - abs(*split_vel)) * signbit(*split_vel) << "\n";
    state->r.rect(*x_pos + 10, *y_pos + 10, *x_pos - 10, *y_pos - 10, Color(55, 55, 55));
}

int main() {
    State state = State("gui", 640, 480);

    Element background = Element("background", update_background);
    state.add_element(&background);

    size_t player_sizes = sizeof(float); 
    Element player1 = Element("player1", &player_sizes, 1, update_player1);
    state.add_element(&player1);

    Element player2 = Element("player2", &player_sizes, 1, update_player2);
    state.add_element(&player2);

    size_t ball_sizes[6] = {sizeof(float*), sizeof(float*), sizeof(float), sizeof(float), sizeof(float), sizeof(float)};
    Element ball = Element("ball", ball_sizes, 6, update_ball, init_ball);
    state.add_element(&ball);

    while (!state.quit) {
        state.update();
    }

    background.destroy();
    player1.destroy();
    player2.destroy();
    ball.destroy();
}