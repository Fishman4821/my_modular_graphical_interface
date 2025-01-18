#include <iostream>
#include <vector>
#include <math.h>

#define SDL_MAIN_HANDLED
#define FPS 60.0f

#include "mgui.cpp"

using namespace std;

void update_background(Element* e, State* state) {
    state->r.rect(0, 0, 640, 480, Color(69, 69, 69));
    state->r.rect(315, 0, 325, 480, Color(60, 60, 60));
}

void init_play_button(Element* e, State* state) {
    Font* font = (Font*)(e->data.value(0));
    *font = Font("RobotoMono-Regular.ttf", 50);
}

void update_play_button(Element* e, State* state) {
    Font* font = (Font*)(e->data.value(0));
    state->r.rect(200, 215, 440, 275, Color(62, 62, 62));
    state->r.text("Play", 270, 220, 100, 50, font, Color(255, 255, 255));
    float mx = state->i.mouse.x;
    float my = state->i.mouse.y;
    if (state->i.mouse.buttons[0] && mx >= 200 && mx <= 440 && my >= 215 && my <= 275) {
        Element* player1 = state->get_element("player1");
        *(float*)player1->data.value(0) = 240;
        player1->enable = true;

        Element* player2 = state->get_element("player2");
        *(float*)player2->data.value(0) = 240;
        player2->enable = true;

        state->get_element("ball")->enable = true;
        state->remove_element("play_button");
    }
}

void update_player1(Element* e, State* state) {
    float* y = (float*)(e->data.value(0));

    state->r.rect(20, *y + 30, 30, *y - 30, Color(40, 40, 40));

    if (state->i.keyboard.keys[SDL_SCANCODE_W] && *y > 40) {
        *y -= 250 * state->t.dt;
    }
    if (state->i.keyboard.keys[SDL_SCANCODE_S] && *y < 440) {
        *y += 250 * state->t.dt;
    }
}

void update_player2(Element* e, State* state) {
    float* y = (float*)(e->data.value(0));
    state->r.rect(610, *y + 30, 620, *y - 30, Color(40, 40, 40));

    if (state->i.keyboard.keys[SDL_SCANCODE_UP] && *y > 40) {
        *y -= 250 * state->t.dt;
    }
    if (state->i.keyboard.keys[SDL_SCANCODE_DOWN] && *y < 440) {
        *y += 250 * state->t.dt;
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
    float* angle = (float*)(e->data.value(4));
    float* speed = (float*)(e->data.value(5));
    *angle = -35;
    *speed = 5;
    *(Font*)(e->data.value(6)) = Font("RobotoMono-Regular.ttf", 50);
    *(int*)(e->data.value(7)) = 0;
    *(int*)(e->data.value(8)) = 0;
}

void update_ball(Element* e, State* state) {
    float* player1_y = *(float**)(e->data.value(0)); // get the pointer to the ball's pointer to the value of player1's y then get the value from that pointer (contains pointer to player1_y)
    float* player2_y = *(float**)(e->data.value(1));
    float* x_pos = ((float*)e->data.value(2));
    float* y_pos = ((float*)e->data.value(3));
    float* angle = (float*)(e->data.value(4));
    float* speed = (float*)(e->data.value(5));
    int* player1_score = (int*)(e->data.value(7));
    int* player2_score = (int*)(e->data.value(8));

    if ((*x_pos - 10 <= 30 && !(*player1_y + 30 < *y_pos - 10) && !(*player1_y - 30 > *y_pos + 10)) || 
        (*x_pos + 10 >= 610 && !(*player2_y + 30 < *y_pos - 10) && !(*player2_y - 30 > *y_pos + 10))) {
        *angle = -*angle;
    }

    if (*y_pos - 10 <= 0) {
        *angle = -*angle + 180;
    } else if (*y_pos + 10 >= 480) {
        *angle = -*angle - 180;
    }

    *x_pos += sin(*angle * M_PI / 180) * *speed;
    *y_pos += cos(*angle * M_PI / 180) * *speed;

    if (*x_pos - 10 <= 0) {
        *player2_score += 1;
        *speed += 0.5;
        *x_pos = 320;
        *y_pos = 240;
    } else if (*x_pos + 10 >= 640) {
        *player1_score += 1;
        *speed += 0.5;
        *x_pos = 320;
        *y_pos = 240;
    }

    //cout << *x_pos << "\t" << *y_pos << "\n";
    state->r.rect(*x_pos + 10, *y_pos + 10, *x_pos - 10, *y_pos - 10, Color(55, 55, 55));

    Font* font = (Font*)(e->data.value(6));
    char player1_score_buf[8];
    itoa(*player1_score, player1_score_buf, 10);
    char player2_score_buf[8];
    itoa(*player2_score, player2_score_buf, 10);

    state->r.text(player1_score_buf, 240, 10, 50, 50, font, Color(255, 255, 255));
    state->r.text(player2_score_buf, 350, 10, 50, 50, font, Color(255, 255, 255));
}

int main() {
    State state = State("pong", 640, 480);

    Element background = Element("background", update_background);
    state.add_element(&background);

    size_t play_button_sizes = sizeof(Font);
    Element play_button = Element("play_button", &play_button_sizes, 1, update_play_button, init_play_button);
    state.add_element(&play_button);

    size_t player_sizes = sizeof(float); 
    Element player1 = Element("player1", &player_sizes, 1, update_player1, false);
    state.add_element(&player1);

    Element player2 = Element("player2", &player_sizes, 1, update_player2, false);
    state.add_element(&player2);

    size_t ball_sizes[9] = {sizeof(float*), sizeof(float*), sizeof(float), sizeof(float), sizeof(float), sizeof(float), sizeof(Font), sizeof(int), sizeof(int)};
    Element ball = Element("ball", ball_sizes, 9, update_ball, init_ball, false);
    state.add_element(&ball);

    while (!state.quit) {
        state.update();
    }

    background.destroy();
    play_button.destroy();
    player1.destroy();
    player2.destroy();
    ball.destroy();
}