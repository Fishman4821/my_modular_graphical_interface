#include <vector>
#include <iostream>

#include "SDL.h"
#include "SDL_ttf.h"

#include "mgui_math.cpp"
#include "mgui_keycodes.cpp"

#ifndef FPS
#define FPS 60.0
#endif

class Color {
public:
    char r, g, b, a;

    Color() = default;
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

    SDL_Color sdl() {
        return {this->r, this->b, this->g, this->a};
    }
};

class Data {
public:
    size_t* sizes;
    void* data;
    int len;

    Data() = default;
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
    bool enable;
    const char* name;
    void (*update_fn)(Element*, State*);
    void (*init_fn)(Element*, State*);
    void (*destruct_fn)(Element*, State*);
    Data data;

    Element() = default;
    Element(const char* name, size_t* sizes, int num_sizes, void (*update_fn)(Element*, State*), void (*init_fn)(Element*, State*), void (*destruct_fn)(Element*, State*), bool enable) {
        this->name = name;
        this->data = Data(sizes, num_sizes);
        this->update_fn = update_fn;
        this->init_fn = init_fn;
        this->destruct_fn = destruct_fn;
        this->enable = enable;
    }

    void update(State* state) {
        (update_fn)(this, state);
    }

    void init(State* state) {
        (init_fn)(this, state);
    }

    void destruct(State* state) {
        (destruct_fn)(this, state);
        this->data.free_data();
    }
};

#ifdef MGUI_TEXT
class Font {
public:
    TTF_Font* font;

    Font(const char* file_path, int point) {
        this->font = TTF_OpenFont(file_path, point);
    }

    void destroy() {
        TTF_CloseFont(this->font);
    }

    void set_size(int point) {
        TTF_SetFontSize(this->font, point);
    }

    void set_style(bool bold, bool italic, bool underline, bool strikethrough) {
        TTF_SetFontStyle(this->font,
            (TTF_STYLE_BOLD & bold) ||
            (TTF_STYLE_ITALIC & italic) ||
            (TTF_STYLE_UNDERLINE & underline) ||
            (TTF_STYLE_STRIKETHROUGH & strikethrough)
        );
    }
};
#endif

enum StateFlags {
    WINDOW_RESIZEABLE = 1,
    INPUT_MULTI_THREADED = 2,
    ELEMENTS_ENABLE = 4,
};

class Renderer {
private:
    SDL_Window* window;
    SDL_Surface* window_surface;
    SDL_Renderer* renderer;

    Uint32 to_sdl_window_flags(Uint32 flags) {
        Uint32 val = 0;
        val = val | ((flags & WINDOW_RESIZEABLE) << 5); 
        return val;
    }

public:
    int w;
    int h;

    Renderer() = default;
    Renderer(const char* windowTitle, int width, int height, int window_flags) {
        SDL_Init(SDL_INIT_VIDEO);
        this->window = SDL_CreateWindow(
            windowTitle,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            to_sdl_window_flags(window_flags));

        this->renderer = SDL_CreateRenderer(window, -1, 0);
        TTF_Init();
        this->window_surface = SDL_GetWindowSurface(this->window);
        SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
    }

    void fullscreen(bool val) {
        SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP & val);
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

#ifdef MGUI_TEXT
    void text(const char* text, int x, int y, int w, int h, Font* font, Color fill) {
        SDL_Surface* text_surface = TTF_RenderText_Blended_Wrapped(font->font, text, fill.sdl(), 0);
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(this->renderer, text_surface);
        SDL_Rect src_rect = {0, 0, text_surface->w, text_surface->h};
        SDL_Rect dst_rect = {x, y, w, h};
        SDL_RenderCopy(this->renderer, text_texture, &src_rect, &dst_rect);
        SDL_FreeSurface(text_surface);
    }
#endif
    void point(int x, int y, Color fill) {
        SDL_SetRenderDrawColor(this->renderer, fill.r, fill.g, fill.b, fill.a);
        SDL_RenderDrawPoint(renderer, x, y);
    }

    void point(Vec2 a, Color fill) {
        SDL_SetRenderDrawColor(this->renderer, fill.r, fill.g, fill.b, fill.a);
        SDL_RenderDrawPoint(renderer, a.x, a.y);
    }

    void line(int x1, int y1, int x2, int y2, Color fill) {
        SDL_SetRenderDrawColor(this->renderer, fill.r, fill.g, fill.b, fill.a);
        SDL_RenderDrawLine(this->renderer, x1, y1, x2, y2);
    }

    void line(Vec2 a, Vec2 b, Color fill) {
        SDL_SetRenderDrawColor(this->renderer, fill.r, fill.g, fill.b, fill.a);
        SDL_RenderDrawLine(this->renderer, a.x, a.y, b.x, b.y);
        
    }

    void cubic_bezier(Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, int precision, Color fill) {
        Vec2 q0 = Vec2(0, 0);
        Vec2 q1 = Vec2(0, 0);
        Vec2 q2 = Vec2(0, 0);

        Vec2 r0 = Vec2(0, 0);
        Vec2 r1 = Vec2(0, 0);

        Vec2 b = Vec2(0, 0);

        for (float t = 0.0f; t < 1.0f; t += 1 / powf(10, precision)) {
            q0 = lerpVec2(p0, p1, t);
            q1 = lerpVec2(p1, p2, t);
            q2 = lerpVec2(p2, p3, t);
        
            r0 = lerpVec2(q0, q1, t);
            r1 = lerpVec2(q1, q2, t);

            b = lerpVec2(r0, r1, t);

            point(b, fill);
        }
    }

    void update() {
        SDL_GetWindowSize(this->window, &this->w, &this->h);
    }

    void draw() {
        SDL_RenderPresent(this->renderer);
    }
};

class Inputs {
private:
    int num_keys;
    struct {
        Uint8* keys;
        ModKeyCode mods;
    } previous_keyboard;

    struct {
        int x, y;
        bool buttons[3];
    } previous_mouse;

public:
    struct {
        const Uint8* keys;
        ModKeyCode mods;
    } keyboard;

    struct {
        int x, y;
        bool buttons[3];
    } mouse;

    Inputs() {
        this->keyboard.keys = SDL_GetKeyboardState(&this->num_keys);
        this->previous_keyboard.keys = (Uint8*)malloc(sizeof(Uint8) * this->num_keys);
    }

    void update_input_start() {
        this->keyboard.keys = SDL_GetKeyboardState(NULL);
        this->keyboard.mods = (ModKeyCode)SDL_GetModState();
        Uint32 mouse_state = SDL_GetMouseState(&this->mouse.x, &this->mouse.y);
        this->mouse.buttons[0] = mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT);
        this->mouse.buttons[1] = mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
        this->mouse.buttons[2] = mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT);
    }

    void update_input_end() {
        memcpy(this->previous_keyboard.keys, this->keyboard.keys, sizeof(Uint8) * this->num_keys);
        this->previous_keyboard.mods = this->keyboard.mods;
        this->previous_mouse.x = mouse.x;
        this->previous_mouse.y = mouse.y;
        this->previous_mouse.buttons[0] = mouse.buttons[0];
        this->previous_mouse.buttons[1] = mouse.buttons[1];
        this->previous_mouse.buttons[2] = mouse.buttons[2];
    }

    bool get_key_pressed(KeyCode key) {
        return (this->keyboard.keys[key] != 0) && (this->previous_keyboard.keys[key] == 0);
    }

    bool get_key_up(KeyCode key) {
        return (this->keyboard.keys[key] == 0) && (this->previous_keyboard.keys[key] != 0);
    }

    bool get_key(KeyCode key) {
        return this->keyboard.keys[key];
    }

    bool get_mod_key_pressed(ModKeyCode key) {
        return (this->keyboard.mods & key != 0) && (this->previous_keyboard.mods & key == 0);
    }

    bool get_mod_key_up(ModKeyCode key) {
        return (this->keyboard.mods & key == 0) && (this->previous_keyboard.mods & key != 0);

    }

    bool get_mod_key(ModKeyCode key) {
        return this->keyboard.mods & key;
    }

    bool get_mouse_button_pressed(MouseButton mb) {
        return (this->mouse.buttons[mb] != 0) && (this->previous_mouse.buttons[mb] == 0);
    }

    bool get_mouse_button_up(MouseButton mb) {
        return (this->mouse.buttons[mb] == 0) && (this->previous_mouse.buttons[mb] != 0);
    }

    bool get_mouse_button(MouseButton mb) {
        return this->mouse.buttons[mb];
    }
};

class Time {
private:
    float old_time;
    float current_time;
public:
    float dt;

    Time() {
        this->old_time = 0.f;
        this->current_time = 0.f;
        this->dt = 0.f;
    }

    void update_dt1() {
        this->old_time = this->current_time;
        this->current_time = SDL_GetPerformanceCounter();
        this->dt = (double)((this->current_time - this->old_time) / (double)SDL_GetPerformanceFrequency());
        if (this->dt > 500.0f) {
            this->dt = 0.f;
        }
    }

    void update_dt2() {
        SDL_Delay(1000.0f / (FPS - this->dt));
    }
};

class State {
private:
    std::vector<Element*> update_list;
    std::vector<Element*> init_list;
    int flags;

public:
    bool quit;

    Renderer r;
    Inputs i;
    Time t;

    State(const char* windowTitle, int width, int height, int flags) {
        this->quit = false;
        this->flags = flags;
        this->r = Renderer(windowTitle, width, height, flags);
        this->t = Time();
        this->i.update_input_start();
        this->i.update_input_end();
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
                this->update_list[i]->destruct(this);
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

    void start_frame() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                this->quit = true;
            } else if (event.type == SDL_WINDOWEVENT) {
                this->r.update();
            }
            break;
        }

        this->t.update_dt1();

        this->i.update_input_start();

        if ((this->flags & ELEMENTS_ENABLE) != 0) {
            this->update_elements();
        }
    }

    void end_frame() {
        this->r.draw();

        this->t.update_dt2();

        this->i.update_input_end();
    }

    void update_elements() {
        for (int i = 0; i < this->init_list.size(); i++) {
            this->init_list[i]->init(this);
        }
        this->init_list.clear();
        for (int i = 0; i < this->update_list.size(); i++) {
            if (this->update_list[i]->enable) {
                this->update_list[i]->update(this);
            }
        }
    }

    void destroy_elements() {
        for (int i = 0; i < this->update_list.size(); i++) {
            this->update_list[i]->destruct(this);
        }
        this->update_list.clear();
    }
};