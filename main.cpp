#include <iostream>
#include <vector>
#include <string.h>

#define SDL_MAIN_HANDLED

#include "mgui.cpp"

using namespace std;

void draw_grid(State* state, int view_x, int view_y, float zoom, int grid_spacing) {
    const Color background = Color(27, 28, 32);
    const Color dot = Color(22, 22, 22);
    const Color line = Color(50, 50, 50, 75);

    int spacing_zoom = grid_spacing * zoom;
    int row_initial = view_y % spacing_zoom;

    int w = state->r.w;
    int h = state->r.h;

    int column = view_x % spacing_zoom;
    int row = row_initial;

    state->r.rect(0, 0, w, h, background);

    while (column - spacing_zoom <= w) {
        while (row - spacing_zoom <= h) {
            state->r.line(column, row, column - spacing_zoom, row, line);
            state->r.line(column, row, column, row - spacing_zoom, line);
            row += spacing_zoom;
        }
        row = row_initial;

        column += spacing_zoom;
    }
    column = view_x % spacing_zoom;
    while (column - spacing_zoom <= w) {
        while (row - spacing_zoom <= h) {
            state->r.rect(column - 1, row - 1, column + 2, row + 2, dot);
            row += spacing_zoom;
        }
        row = row_initial;

        column += spacing_zoom;
    }

}

struct Wire {
    int x1, y1;
    int x2, y2;
    bool selected;
    Wire* prev;
    Wire* next;
};

void delete_wire(Wire** wires, Wire* wire) {
    if (wire->prev == nullptr) {
        *wires = wire->next;
    } else {
        wire->prev->next = wire->next;
    }
    if (wire->next != nullptr) {
        wire->next->prev = wire->prev;
    }
    free(wire);
}

void append_wire(Wire** wires, Wire wire) {
    Wire* next = (Wire*)malloc(sizeof(Wire));
    *next = wire;
    if (*wires == nullptr) {
        *wires = next;
        return;
    }
    Wire* end;
    Wire* current = *wires;
    while (current != nullptr) {
        end = current;
        current = current->next;
    }
    end->next = next;
    next->prev = end;
}

void free_wires(Wire* wires) {
    Wire* current = wires;
    Wire* temp;
    while (current != nullptr) {
        temp = current->next;
        free(current);
        current = temp;
    }
}

struct Node {
    char state;
    Wire* wires;
    Node* prev;
    Node* next;
};

void delete_node(Node** nodes, Node* node) {
    if (node->prev == nullptr) {
        *nodes = node->next;
    } else {
        node->prev->next = node->next;
    }
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    }
    free_wires(node->wires);
    free(node);
}

void append_node(Node** nodes, Node node) {
    Node* next = (Node*)malloc(sizeof(Node));
    *next = node;
    if (*nodes == nullptr) {
        *nodes = next;
        return;
    }
    Node* end;
    Node* current = *nodes;
    while (current != nullptr) {
        end = current;
        current = current->next;
    }
    end->next = next;
    next->prev = end;
}

void free_nodes(Node* nodes) {
    Node* current = nodes;
    Node* temp;
    while (current != nullptr) {
        temp = current->next;
        free_wires(current->wires);
        free(current);
        current = temp;
    }
}

Node* get_node(Node* nodes, int index) {
    static int _i = 0;
    Node* current = nodes;
    for (_i = 0; _i < index; _i++) {
        if (current->next != nullptr) { 
            current = current->next;
        } else {
            break;
        }
    } 
    return current;
}

struct Object {
    char type;
    bool state;
    int x, y;
    char rotation;
    int a, b, c;
    bool selected;
    Object* prev;
    Object* next;
};

void delete_object(Object** objects, Object* object) {
    if (object->prev == nullptr) {
        *objects = object->next;
    } else {
        object->prev->next = object->next;
    }
    if (object->next != nullptr) {
        object->next->prev = object->prev;
    }
    free(object);
}

void append_object(Object** objects, Object object) {
    Object* next = (Object*)malloc(sizeof(Object));
    *next = object;
    if (*objects == nullptr) {
        *objects = next;
        return;
    }
    Object* end;
    Object* previous;
    Object* current = *objects;
    while (current != nullptr) {
        end = current;
        current = current->next;
    }
    end->next = next;
    next->prev = end;
}

void free_objects(Object* objects) {
    Object* current = objects;
    Object* temp;
    while (current != nullptr) {
        temp = current->next;
        free(current);
        current = temp;
    }
}

const Color wire_disconnected = Color(66, 60, 68);
const Color wire_on = Color(50, 150, 0);
const Color wire_off = Color(0, 65, 0);
const Color wire_conflicted = Color(140, 140, 0);

const Color selection = Color(255, 255, 255, 35);

void draw_nmos_symbol(State* state, int x, int y, char rot, int view_x, int view_y, int zoom, bool selected, Color a, Color b) {
    if (rot == 0 || rot == 2) {
        int top_y = y * zoom;
        int bottom_y = top_y + zoom;
        int bottom_1_3_y = bottom_y - zoom / 3.0;
        int left_x = x * zoom;
        int left_1_3_x = left_x + zoom / 3.0;
        int right_x = x * zoom + (zoom * 2);
        int right_1_3_x = right_x - zoom / 3.0;
        int left_1_2_x = left_x + zoom / 2.0;
        int right_1_2_x = right_x - zoom / 2.0;
        int middle_y = bottom_y - zoom / 2.0;
        int middle_x = left_x + zoom;
        int top_1_3_y = top_y + zoom / 3.0;
        if (rot == 0) {
            state->r.line(left_x + view_x, bottom_y + view_y, left_1_3_x + view_x, bottom_y + view_y, a);
            state->r.line(left_1_3_x + view_x, bottom_y + view_y, left_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(left_1_3_x + view_x, bottom_1_3_y + view_y, right_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(right_1_3_x + view_x, bottom_y + view_y, right_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(right_x + view_x, bottom_y + view_y, right_1_3_x + view_x, bottom_y + view_y, a);

            state->r.line(left_1_2_x + view_x, middle_y + view_y, right_1_2_x + view_x, middle_y + view_y, b);
            state->r.line(middle_x + view_x, middle_y + view_y, middle_x + view_x, top_y + view_y, b);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        } else {
            state->r.line(left_x + view_x, top_y + view_y, left_1_3_x + view_x, top_y + view_y, a);
            state->r.line(left_1_3_x + view_x, top_y + view_y, left_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(left_1_3_x + view_x, top_1_3_y + view_y, right_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(right_1_3_x + view_x, top_y + view_y, right_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(right_x + view_x, top_y + view_y, right_1_3_x + view_x, top_y + view_y, a);

            state->r.line(left_1_2_x + view_x, middle_y + view_y, right_1_2_x + view_x, middle_y + view_y, b);
            state->r.line(middle_x + view_x, middle_y + view_y, middle_x + view_x, bottom_y + view_y, b);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        }
    } else {
        int left_x = x * zoom;
        int right_x = left_x + zoom;
        int left_1_3_x = left_x + zoom / 3.0;
        int top_y = y * zoom;
        int top_1_3_y = top_y + zoom / 3.0;
        int bottom_y = y * zoom + (zoom * 2);
        int bottom_1_3_y = bottom_y - zoom / 3.0;
        int top_1_2_y = top_y + zoom / 2.0;
        int bottom_1_2_y = bottom_y - zoom / 2.0;
        int middle_x = right_x - zoom / 2.0;
        int middle_y = top_y + zoom;
        int right_1_3_x = right_x - zoom / 3.0;
        if (rot == 1) {
            state->r.line(left_x + view_x, top_y + view_y, left_x + view_x, top_1_3_y + view_y, a);
            state->r.line(left_x + view_x, top_1_3_y + view_y, left_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(left_1_3_x + view_x, top_1_3_y + view_y, left_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(left_x + view_x, bottom_1_3_y + view_y, left_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(left_x + view_x, bottom_y + view_y, left_x + view_x, bottom_1_3_y + view_y, a);

            state->r.line(middle_x + view_x, top_1_2_y + view_y, middle_x + view_x, bottom_1_2_y + view_y, b);
            state->r.line(middle_x + view_x, middle_y + view_y, right_x + view_x, middle_y + view_y, b);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        } else {
            state->r.line(right_x + view_x, top_y + view_y, right_x + view_x, top_1_3_y + view_y, a);
            state->r.line(right_x + view_x, top_1_3_y + view_y, right_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(right_1_3_x + view_x, top_1_3_y + view_y, right_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(right_x + view_x, bottom_1_3_y + view_y, right_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(right_x + view_x, bottom_y + view_y, right_x + view_x, bottom_1_3_y + view_y, a);

            state->r.line(middle_x + view_x, top_1_2_y + view_y, middle_x + view_x, bottom_1_2_y + view_y, b);
            state->r.line(middle_x + view_x, middle_y + view_y, left_x + view_x, middle_y + view_y, b);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        }
    }
}

void draw_nmos(State* state, Object* object, Node* nodes, int view_x, int view_y, int spacing_zoom) {
    Node* a = get_node(nodes, object->a);
    const Color* a_color;
    Node* b = get_node(nodes, object->b);
    const Color* b_color;
    if (a->state == 0b00) {
        a_color = &wire_disconnected;
    } else if (a->state == 0b01) {
        a_color = &wire_off;
    } else if (a->state == 0b10) {
        a_color = &wire_on;
    } else if (a->state == 0b11) {
        a_color = &wire_conflicted;
    }

    if (b->state == 0b00) {
        b_color = &wire_disconnected;
    } else if (b->state == 0b01) {
        b_color = &wire_off;
    } else if (b->state == 0b10) {
        b_color = &wire_on;
    } else if (b->state == 0b11) {
        b_color = &wire_conflicted;
    }

    draw_nmos_symbol(state, object->x, object->y, object->rotation, view_x, view_y, spacing_zoom, object->selected, *a_color, *b_color);
}

void draw_pmos_symbol(State* state, int x, int y, char rot, int view_x, int view_y, int zoom, bool selected, Color a, Color b) {
    if (rot == 0 || rot == 2) {
        int top_y = y * zoom;
        int bottom_y = top_y + zoom;
        int bottom_1_3_y = bottom_y - zoom / 3.0;
        int left_x = x * zoom;
        int left_1_3_x = left_x + zoom / 3.0;
        int right_x = x * zoom + (zoom * 2);
        int right_1_3_x = right_x - zoom / 3.0;
        int left_1_2_x = left_x + zoom / 2.0;
        int right_1_2_x = right_x - zoom / 2.0;
        int middle_y = bottom_y - zoom / 2.0;
        int middle_x = left_x + zoom;
        int top_1_3_y = top_y + zoom / 3.0;
        int zoom_1_12 = zoom / 12.0f;
        if (rot == 0) {
            state->r.line(left_x + view_x, bottom_y + view_y, left_1_3_x + view_x, bottom_y + view_y, a);
            state->r.line(left_1_3_x + view_x, bottom_y + view_y, left_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(left_1_3_x + view_x, bottom_1_3_y + view_y, right_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(right_1_3_x + view_x, bottom_y + view_y, right_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(right_x + view_x, bottom_y + view_y, right_1_3_x + view_x, bottom_y + view_y, a);

            state->r.line(left_1_2_x + view_x, middle_y + view_y, right_1_2_x + view_x, middle_y + view_y, b);
            state->r.line(middle_x + view_x, top_1_3_y + view_y, middle_x + view_x, top_y + view_y, b);

            state->r.line(middle_x + view_x, top_1_3_y + view_y, middle_x - zoom_1_12 + view_x, top_1_3_y + zoom_1_12 + view_y, b);
            state->r.line(middle_x - zoom_1_12 + view_x, top_1_3_y + zoom_1_12 + view_y, middle_x + view_x, middle_y + view_y, b);
            state->r.line(middle_x + view_x, middle_y + view_y, middle_x + zoom_1_12 + view_x, top_1_3_y + zoom_1_12 + view_y, b);
            state->r.line(middle_x + zoom_1_12 + view_x, top_1_3_y + zoom_1_12 + view_y, middle_x + view_x, top_1_3_y + view_y, b);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        } else {
            state->r.line(left_x + view_x, top_y + view_y, left_1_3_x + view_x, top_y + view_y, a);
            state->r.line(left_1_3_x + view_x, top_y + view_y, left_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(left_1_3_x + view_x, top_1_3_y + view_y, right_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(right_1_3_x + view_x, top_y + view_y, right_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(right_x + view_x, top_y + view_y, right_1_3_x + view_x, top_y + view_y, a);

            state->r.line(left_1_2_x + view_x, middle_y + view_y, right_1_2_x + view_x, middle_y + view_y, b);
            state->r.line(middle_x + view_x, bottom_1_3_y + view_y, middle_x + view_x, bottom_y + view_y, b);

            state->r.line(middle_x + view_x, bottom_1_3_y + view_y, middle_x - zoom_1_12 + view_x, bottom_1_3_y - zoom_1_12 + view_y, b);
            state->r.line(middle_x - zoom_1_12 + view_x, bottom_1_3_y - zoom_1_12 + view_y, middle_x + view_x, middle_y + view_y, b);
            state->r.line(middle_x + view_x, middle_y + view_y, middle_x + zoom_1_12 + view_x, bottom_1_3_y - zoom_1_12 + view_y, b);
            state->r.line(middle_x + zoom_1_12 + view_x, bottom_1_3_y - zoom_1_12 + view_y, middle_x + view_x, bottom_1_3_y + view_y, b);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        }
    } else {
        int left_x = x * zoom;
        int right_x = left_x + zoom;
        int left_1_3_x = left_x + zoom / 3.0;
        int top_y = y * zoom;
        int top_1_3_y = top_y + zoom / 3.0;
        int bottom_y = y * zoom + (zoom * 2);
        int bottom_1_3_y = bottom_y - zoom / 3.0;
        int top_1_2_y = top_y + zoom / 2.0;
        int bottom_1_2_y = bottom_y - zoom / 2.0;
        int middle_x = right_x - zoom / 2.0;
        int middle_y = top_y + zoom;
        int right_1_3_x = right_x - zoom / 3.0;
        int zoom_1_12 = zoom / 12.0;
        if (rot == 1) {
            state->r.line(left_x + view_x, top_y + view_y, left_x + view_x, top_1_3_y + view_y, a);
            state->r.line(left_x + view_x, top_1_3_y + view_y, left_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(left_1_3_x + view_x, top_1_3_y + view_y, left_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(left_x + view_x, bottom_1_3_y + view_y, left_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(left_x + view_x, bottom_y + view_y, left_x + view_x, bottom_1_3_y + view_y, a);

            state->r.line(middle_x + view_x, top_1_2_y + view_y, middle_x + view_x, bottom_1_2_y + view_y, b);
            state->r.line(right_1_3_x + view_x, middle_y + view_y, right_x + view_x, middle_y + view_y, b);

            state->r.line(right_1_3_x + view_x, middle_y + view_y, right_1_3_x - zoom_1_12 + view_x, middle_y - zoom_1_12 + view_y, b);
            state->r.line(right_1_3_x - zoom_1_12 + view_x, middle_y - zoom_1_12 + view_y, middle_x + view_x, middle_y + view_y, b);
            state->r.line(middle_x + view_x, middle_y + view_y, right_1_3_x - zoom_1_12 + view_x, middle_y + zoom_1_12 + view_y, b);
            state->r.line(right_1_3_x - zoom_1_12 + view_x, middle_y + zoom_1_12 + view_y, right_1_3_x + view_x, middle_y + view_y, b);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        } else {
            state->r.line(right_x + view_x, top_y + view_y, right_x + view_x, top_1_3_y + view_y, a);
            state->r.line(right_x + view_x, top_1_3_y + view_y, right_1_3_x + view_x, top_1_3_y + view_y, a);
            state->r.line(right_1_3_x + view_x, top_1_3_y + view_y, right_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(right_x + view_x, bottom_1_3_y + view_y, right_1_3_x + view_x, bottom_1_3_y + view_y, a);
            state->r.line(right_x + view_x, bottom_y + view_y, right_x + view_x, bottom_1_3_y + view_y, a);

            state->r.line(middle_x + view_x, top_1_2_y + view_y, middle_x + view_x, bottom_1_2_y + view_y, b);
            state->r.line(left_1_3_x + view_x, middle_y + view_y, left_x + view_x, middle_y + view_y, b);

            state->r.line(left_1_3_x + view_x, middle_y + view_y, left_1_3_x + zoom_1_12 + view_x, middle_y - zoom_1_12 + view_y, b);
            state->r.line(left_1_3_x + zoom_1_12 + view_x, middle_y - zoom_1_12 + view_y, middle_x + view_x, middle_y + view_y, b);
            state->r.line(middle_x + view_x, middle_y + view_y, left_1_3_x + zoom_1_12 + view_x, middle_y + zoom_1_12 + view_y, b);
            state->r.line(left_1_3_x + zoom_1_12 + view_x, middle_y + zoom_1_12 + view_y, left_1_3_x + view_x, middle_y + view_y, b);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        }
    }
}

void draw_pmos(State* state, Object* object, Node* nodes, int view_x, int view_y, int spacing_zoom) {
    Node* a = get_node(nodes, object->a);
    const Color* a_color;
    Node* b = get_node(nodes, object->b);
    const Color* b_color;
    if (a->state == 0b00) {
        a_color = &wire_disconnected;
    } else if (a->state == 0b01) {
        a_color = &wire_off;
    } else if (a->state == 0b10) {
        a_color = &wire_on;
    } else if (a->state == 0b11) {
        a_color = &wire_conflicted;
    }

    if (b->state == 0b00) {
        b_color = &wire_disconnected;
    } else if (b->state == 0b01) {
        b_color = &wire_off;
    } else if (b->state == 0b10) {
        b_color = &wire_on;
    } else if (b->state == 0b11) {
        b_color = &wire_conflicted;
    }

    draw_pmos_symbol(state, object->x, object->y, object->rotation, view_x, view_y, spacing_zoom, object->selected, *a_color, *b_color);
}

void draw_vplus_symbol(State* state, int x, int y, char rot, int view_x, int view_y, int zoom, bool selected, Color a) {
    if (rot == 0 || rot == 2) {
        int middle_x = x * zoom;
        int left_x = middle_x - zoom * 0.176777; // 0.176777 = sqrt(2) / 8
        int right_x = middle_x + zoom * 0.176777;
        if (rot == 0) {
            int bottom_y = y * zoom;
            int middle_y = bottom_y - zoom / 2.0;
            int top_y = bottom_y - zoom * 0.707107; // 0.707107 = sqrt(2) / 2

            state->r.line(middle_x + view_x, bottom_y + view_y, middle_x + view_x, middle_y + view_y, a);
            state->r.line(left_x + view_x, middle_y + view_y, right_x + view_x, middle_y + view_y, a);
            state->r.line(left_x + view_x, middle_y + view_y, middle_x + view_x, top_y + view_y, a);
            state->r.line(right_x + view_x, middle_y + view_y, middle_x + view_x, top_y + view_y, a);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        } else {
            int top_y = y * zoom;
            int middle_y = top_y + zoom / 2.0;
            int bottom_y = top_y + zoom * 0.707107; // 0.707107 = sqrt(2) / 2

            state->r.line(middle_x + view_x, top_y + view_y, middle_x + view_x, middle_y + view_y, a);
            state->r.line(left_x + view_x, middle_y + view_y, right_x + view_x, middle_y + view_y, a);
            state->r.line(left_x + view_x, middle_y + view_y, middle_x + view_x, bottom_y + view_y, a);
            state->r.line(right_x + view_x, middle_y + view_y, middle_x + view_x, bottom_y + view_y, a);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y - 3, right_x + view_x + 3, bottom_y + view_y + 3, selection); }
        }
    } else {
        int middle_y = y * zoom;
        int bottom_y = middle_y - zoom * 0.176777; // 0.176777 = sqrt(2) / 8
        int top_y = middle_y + zoom * 0.176777;
        if (rot == 1) {
            int left_x = x * zoom;
            int middle_x = left_x + zoom / 2.0;
            int right_x = left_x + zoom * 0.707107; // 0.707107 = sqrt(2) / 2

            state->r.line(left_x + view_x, middle_y + view_y, middle_x + view_x, middle_y + view_y, a);
            state->r.line(middle_x + view_x, top_y + view_y, middle_x + view_x, bottom_y + view_y, a);
            state->r.line(middle_x + view_x, top_y + view_y, right_x + view_x, middle_y + view_y, a);
            state->r.line(middle_x + view_x, bottom_y + view_y, right_x + view_x, middle_y + view_y, a);
            if (selected) { state->r.rect(left_x + view_x - 3, top_y + view_y + 3, right_x + view_x + 3, bottom_y + view_y - 3, selection); }
        } else {
            int left_x = x * zoom;
            int middle_x = left_x - zoom / 2.0;
            int right_x = left_x - zoom * 0.707107; // 0.707107 = sqrt(2) / 2

            state->r.line(left_x + view_x, middle_y + view_y, middle_x + view_x, middle_y + view_y, a);
            state->r.line(middle_x + view_x, top_y + view_y, middle_x + view_x, bottom_y + view_y, a);
            state->r.line(middle_x + view_x, top_y + view_y, right_x + view_x, middle_y + view_y, a);
            state->r.line(middle_x + view_x, bottom_y + view_y, right_x + view_x, middle_y + view_y, a);
            if (selected) { state->r.rect(left_x + view_x + 3, top_y + view_y + 3, right_x + view_x - 3, bottom_y + view_y - 3, selection); }
        }
    }
}

void draw_vplus(State* state, Object* object, Node* nodes, int view_x, int view_y, int spacing_zoom) {
    draw_vplus_symbol(state, object->x, object->y, object->rotation, view_x, view_y, spacing_zoom, object->selected, wire_on);
}

void draw_vminus_symbol(State* state, int x, int y, char rot, int view_x, int view_y, int zoom, bool selected, Color a) {
    if (rot == 0 || rot == 2) {
        int middle_x = x * zoom;
        int t_1_x = zoom * 0.176777; // 0.176777 = sqrt(2) / 8
        int t_2_x = zoom * 0.117851; // 0.117851 = sqrt(2) / 12
        int t_3_x = zoom * 0.058926; // 0.058926 = sqrt(2) / 24
        int start_y = y * zoom;
        int t_1_y = zoom / 2.0;
        int t_2_y = t_1_y + zoom * 0.078567; // 0.078567 = sqrt(2) / 18
        int t_3_y = t_1_y + zoom * 0.141421; // 0.141421 = sqrt(2) / 10

        if (rot == 0) {
            state->r.line(middle_x + view_x, start_y + view_y, middle_x + view_x, start_y - t_1_y + view_y, a);
            state->r.line(middle_x - t_1_x + view_x, start_y - t_1_y + view_y, middle_x + t_1_x + view_x, start_y - t_1_y + view_y, a);
            state->r.line(middle_x - t_2_x + view_x, start_y - t_2_y + view_y, middle_x + t_2_x + view_x, start_y - t_2_y + view_y, a);
            state->r.line(middle_x - t_3_x + view_x, start_y - t_3_y + view_y, middle_x + t_3_x + view_x, start_y - t_3_y + view_y, a);
            if (selected) { state->r.rect(middle_x - t_1_x + view_x - 3, start_y - t_3_y + view_y - 3, middle_x + t_1_x + view_x + 3, start_y + view_y + 3, selection); }
        } else {
            state->r.line(middle_x + view_x, start_y + view_y, middle_x + view_x, start_y + t_1_y + view_y, a);
            state->r.line(middle_x - t_1_x + view_x, start_y + t_1_y + view_y, middle_x + t_1_x + view_x, start_y + t_1_y + view_y, a);
            state->r.line(middle_x - t_2_x + view_x, start_y + t_2_y + view_y, middle_x + t_2_x + view_x, start_y + t_2_y + view_y, a);
            state->r.line(middle_x - t_3_x + view_x, start_y + t_3_y + view_y, middle_x + t_3_x + view_x, start_y + t_3_y + view_y, a);
            if (selected) { state->r.rect(middle_x - t_1_x + view_x - 3, start_y + view_y - 3, middle_x + t_1_x + view_x + 3, start_y + t_3_y + view_y + 3, selection); }
        }
    } else {
        int middle_y = y * zoom;
        int t_1_y = zoom * 0.176777; // 0.176777 = sqrt(2) / 8
        int t_2_y = zoom * 0.117851; // 0.117851 = sqrt(2) / 12
        int t_3_y = zoom * 0.058926; // 0.058926 = sqrt(2) / 24
        int start_x = x * zoom;
        int t_1_x = zoom / 2.0;
        int t_2_x = t_1_x + zoom * 0.078567; // 0.078567 = sqrt(2) / 18
        int t_3_x = t_1_x + zoom * 0.141421; // 0.141421 = sqrt(2) / 10

        if (rot == 1) {
            state->r.line(start_x + view_x, middle_y + view_y, start_x + t_1_x + view_x, middle_y + view_y, a);
            state->r.line(start_x + t_1_x + view_x, middle_y - t_1_y + view_y, start_x + t_1_x + view_x, middle_y + t_1_y + view_y, a);
            state->r.line(start_x + t_2_x + view_x, middle_y - t_2_y + view_y, start_x + t_2_x + view_x, middle_y + t_2_y + view_y, a);
            state->r.line(start_x + t_3_x + view_x, middle_y - t_3_y + view_y, start_x + t_3_x + view_x, middle_y + t_3_y + view_y, a);
            if (selected) { state->r.rect(start_x + view_x - 3, middle_y - t_1_y + view_y - 3, start_x + t_3_x + view_x + 3, middle_y + t_1_y + view_y + 3, selection); }
        } else {
            state->r.line(start_x + view_x, middle_y + view_y, start_x - t_1_x + view_x, middle_y + view_y, a);
            state->r.line(start_x - t_1_x + view_x, middle_y - t_1_y + view_y, start_x - t_1_x + view_x, middle_y + t_1_y + view_y, a);
            state->r.line(start_x - t_2_x + view_x, middle_y - t_2_y + view_y, start_x - t_2_x + view_x, middle_y + t_2_y + view_y, a);
            state->r.line(start_x - t_3_x + view_x, middle_y - t_3_y + view_y, start_x - t_3_x + view_x, middle_y + t_3_y + view_y, a);
            if (selected) { state->r.rect(start_x - t_3_x + view_x - 3, middle_y - t_1_y + view_y - 3, start_x + view_x + 3, middle_y + t_1_y + view_y + 3, selection); }
        }
    }
}

void draw_vminus(State* state, Object* object, Node* nodes, int view_x, int view_y, int spacing_zoom) {
    draw_vminus_symbol(state, object->x, object->y, object->rotation, view_x, view_y, spacing_zoom, object->selected, wire_off);
}

void draw_input_symbol(State* state, int x, int y, char rot, int view_x, int view_y, int zoom, bool selected, Color a, Color b) {
    int start_x = x * zoom + view_x;
    int start_y = y * zoom + view_y;
    int zoom_1_9 = zoom / 9.0;
    int zoom_1_6 = zoom / 6.0;
    int zoom_1_3 = zoom / 3.0;
    int zoom_2_3 = zoom * (2.0 / 3.0);

    if (rot == 0 || rot == 2) {
        if (rot == 0) {
            state->r.line(start_x, start_y, start_x + zoom_1_6, start_y - zoom_1_6, b);
            state->r.line(start_x + zoom_1_6, start_y - zoom_1_6, start_x + zoom_1_6, start_y - zoom_2_3, b);
            state->r.line(start_x, start_y, start_x - zoom_1_6, start_y - zoom_1_6, b);
            state->r.line(start_x - zoom_1_6, start_y - zoom_1_6, start_x - zoom_1_6, start_y - zoom_2_3, b);
            state->r.line(start_x + zoom_1_6, start_y - zoom_2_3, start_x - zoom_1_6, start_y - zoom_2_3, b);

            state->r.line(start_x, start_y, start_x, start_y - zoom_1_3, a);
            state->r.rect(start_x + zoom_1_9, start_y - zoom_1_3 - zoom_1_9 * 2, start_x - zoom_1_9, start_y - zoom_1_3, a);
            if (selected) { state->r.rect(start_x - zoom_1_6 - 3, start_y - zoom_2_3 - 3, start_x + zoom_1_6 + 3, start_y + 3, selection); }
        } else {
            state->r.line(start_x, start_y, start_x + zoom_1_6, start_y + zoom_1_6, b);
            state->r.line(start_x + zoom_1_6, start_y + zoom_1_6, start_x + zoom_1_6, start_y + zoom_2_3, b);
            state->r.line(start_x, start_y, start_x - zoom_1_6, start_y + zoom_1_6, b);
            state->r.line(start_x - zoom_1_6, start_y + zoom_1_6, start_x - zoom_1_6, start_y + zoom_2_3, b);
            state->r.line(start_x + zoom_1_6, start_y + zoom_2_3, start_x - zoom_1_6, start_y + zoom_2_3, b);

            state->r.line(start_x, start_y, start_x, start_y + zoom_1_3, a);
            state->r.rect(start_x + zoom_1_9, start_y + zoom_1_3 + zoom_1_9 * 2, start_x - zoom_1_9, start_y + zoom_1_3, a);
            if (selected) { state->r.rect(start_x - zoom_1_6 - 3, start_y - 3, start_x + zoom_1_6 + 3, start_y + zoom_2_3 + 3, selection); }
        }
    } else {
        if (rot == 1) {
            state->r.line(start_x, start_y, start_x + zoom_1_6, start_y + zoom_1_6, b);
            state->r.line(start_x + zoom_1_6, start_y + zoom_1_6, start_x + zoom_2_3, start_y + zoom_1_6, b);
            state->r.line(start_x, start_y, start_x + zoom_1_6, start_y - zoom_1_6, b);
            state->r.line(start_x + zoom_1_6, start_y - zoom_1_6, start_x + zoom_2_3, start_y - zoom_1_6, b);
            state->r.line(start_x + zoom_2_3, start_y + zoom_1_6, start_x + zoom_2_3, start_y - zoom_1_6, b);

            state->r.line(start_x, start_y, start_x + zoom_1_3, start_y, a);
            state->r.rect(start_x + zoom_1_3 + zoom_1_9 * 2, start_y + zoom_1_9, start_x + zoom_1_3, start_y - zoom_1_9, a);
            if (selected) { state->r.rect(start_x - 3, start_y - zoom_1_6 - 3, start_x + zoom_2_3 + 3, start_y + zoom_1_6 + 3, selection); }
        } else {
            state->r.line(start_x, start_y, start_x - zoom_1_6, start_y + zoom_1_6, b);
            state->r.line(start_x - zoom_1_6, start_y + zoom_1_6, start_x - zoom_2_3, start_y + zoom_1_6, b);
            state->r.line(start_x, start_y, start_x - zoom_1_6, start_y - zoom_1_6, b);
            state->r.line(start_x - zoom_1_6, start_y - zoom_1_6, start_x - zoom_2_3, start_y - zoom_1_6, b);
            state->r.line(start_x - zoom_2_3, start_y + zoom_1_6, start_x - zoom_2_3, start_y - zoom_1_6, b);

            state->r.line(start_x, start_y, start_x - zoom_1_3, start_y, a);
            state->r.rect(start_x - zoom_1_3 - zoom_1_9 * 2, start_y + zoom_1_9, start_x - zoom_1_3, start_y - zoom_1_9, a);
            if (selected) { state->r.rect(start_x - zoom_2_3 - 3, start_y - zoom_1_6 - 3, start_x + 3, start_y + zoom_1_6 + 3, selection); }
        }
    }
}

void draw_input(State* state, Object* object, Node* nodes, int view_x, int view_y, int spacing_zoom) {
    const Color* color;
    if (object->state) { color = &wire_on; }
    else { color = &wire_off; }

    draw_input_symbol(state, object->x, object->y, object->rotation, view_x, view_y, spacing_zoom, object->selected, *color, Color(0, 0, 0));
}

void draw_output_symbol(State* state, int x, int y, char rot, int view_x, int view_y, int zoom, bool selected, Color a, Color b) {
    const float sin_table[25] = {0, -0.258819, -0.5, -0.707107, -0.866025, -0.965926, -1, -0.965926, -0.866025, -0.707107, -0.5, -0.258819, 0, 0.258819, 0.5, 0.707107, 0.866025, 0.965926, 1, 0.965926, 0.866025, 0.707107, 0.5, 0.258819, 0};
    const float cos_table[25] = {1, 0.965926, 0.866025, 0.707107, 0.5, 0.258819, 0, -0.258819, -0.5, -0.707107, -0.866025, -0.965926, -1, -0.965926, -0.866025, -0.707107, -0.5, -0.258819, 0, 0.258819, 0.5, 0.707107, 0.866025, 0.965926, 1};
    int start_x = x * zoom + view_x;
    int start_y = y * zoom + view_y;
    int zoom_1_9 = zoom / 9.0;
    int zoom_1_6 = zoom / 6.0;
    int zoom_1_3 = zoom / 3.0;
    int zoom_2_3 = zoom * (2.0 / 3.0);

    if (rot == 0 || rot == 2) {
        if (rot == 0) {
            state->r.line(start_x, start_y, start_x + zoom_1_6, start_y - zoom_1_6, b);
            state->r.line(start_x + zoom_1_6, start_y - zoom_1_6, start_x + zoom_1_6, start_y - zoom_2_3, b);
            state->r.line(start_x, start_y, start_x - zoom_1_6, start_y - zoom_1_6, b);
            state->r.line(start_x - zoom_1_6, start_y - zoom_1_6, start_x - zoom_1_6, start_y - zoom_2_3, b);
            state->r.line(start_x + zoom_1_6, start_y - zoom_2_3, start_x - zoom_1_6, start_y - zoom_2_3, b);

            state->r.line(start_x, start_y, start_x, start_y - zoom_1_3, a);
            for (int i = 0; i < 24; i++) {
                state->r.triangle(start_x, start_y - zoom_1_3 - zoom_1_9, 
                                  start_x + sin_table[i] * zoom_1_9, start_y - zoom_1_3 - zoom_1_9 + cos_table[i] * zoom_1_9,
                                  start_x + sin_table[i + 1] * zoom_1_9, start_y - zoom_1_3 - zoom_1_9 + cos_table[i + 1] * zoom_1_9, a);
            }
            if (selected) { state->r.rect(start_x - zoom_1_6 - 3, start_y - zoom_2_3 - 3, start_x + zoom_1_6 + 3, start_y + 3, selection); }
        } else {
            state->r.line(start_x, start_y, start_x + zoom_1_6, start_y + zoom_1_6, b);
            state->r.line(start_x + zoom_1_6, start_y + zoom_1_6, start_x + zoom_1_6, start_y + zoom_2_3, b);
            state->r.line(start_x, start_y, start_x - zoom_1_6, start_y + zoom_1_6, b);
            state->r.line(start_x - zoom_1_6, start_y + zoom_1_6, start_x - zoom_1_6, start_y + zoom_2_3, b);
            state->r.line(start_x + zoom_1_6, start_y + zoom_2_3, start_x - zoom_1_6, start_y + zoom_2_3, b);

            state->r.line(start_x, start_y, start_x, start_y + zoom_1_3, a);
            for (int i = 0; i < 24; i++) {
                state->r.triangle(start_x, start_y + zoom_1_3 + zoom_1_9, 
                                  start_x + sin_table[i] * zoom_1_9, start_y + zoom_1_3 + zoom_1_9 + cos_table[i] * zoom_1_9,
                                  start_x + sin_table[i + 1] * zoom_1_9, start_y + zoom_1_3 + zoom_1_9 + cos_table[i + 1] * zoom_1_9, a);
            }
            if (selected) { state->r.rect(start_x - zoom_1_6 - 3, start_y - 3, start_x + zoom_1_6 + 3, start_y + zoom_2_3 + 3, selection); }
        }
    } else {
        if (rot == 1) {
            state->r.line(start_x, start_y, start_x + zoom_1_6, start_y + zoom_1_6, b);
            state->r.line(start_x + zoom_1_6, start_y + zoom_1_6, start_x + zoom_2_3, start_y + zoom_1_6, b);
            state->r.line(start_x, start_y, start_x + zoom_1_6, start_y - zoom_1_6, b);
            state->r.line(start_x + zoom_1_6, start_y - zoom_1_6, start_x + zoom_2_3, start_y - zoom_1_6, b);
            state->r.line(start_x + zoom_2_3, start_y + zoom_1_6, start_x + zoom_2_3, start_y - zoom_1_6, b);

            state->r.line(start_x, start_y, start_x + zoom_1_3, start_y, a);
            for (int i = 0; i < 24; i++) {
                state->r.triangle(start_x + zoom_1_3 + zoom_1_9, start_y, 
                                  start_x + zoom_1_3 + zoom_1_9 + sin_table[i] * zoom_1_9, start_y + cos_table[i] * zoom_1_9,
                                  start_x + zoom_1_3 + zoom_1_9 + sin_table[i + 1] * zoom_1_9, start_y + cos_table[i + 1] * zoom_1_9, a);
            }
            if (selected) { state->r.rect(start_x - 3, start_y - zoom_1_6 - 3, start_x + zoom_2_3 + 3, start_y + zoom_1_6 + 3, selection); }
        } else {
            state->r.line(start_x, start_y, start_x - zoom_1_6, start_y + zoom_1_6, b);
            state->r.line(start_x - zoom_1_6, start_y + zoom_1_6, start_x - zoom_2_3, start_y + zoom_1_6, b);
            state->r.line(start_x, start_y, start_x - zoom_1_6, start_y - zoom_1_6, b);
            state->r.line(start_x - zoom_1_6, start_y - zoom_1_6, start_x - zoom_2_3, start_y - zoom_1_6, b);
            state->r.line(start_x - zoom_2_3, start_y + zoom_1_6, start_x - zoom_2_3, start_y - zoom_1_6, b);

            state->r.line(start_x, start_y, start_x - zoom_1_3, start_y, a);
            for (int i = 0; i < 24; i++) {
                state->r.triangle(start_x - zoom_1_3 - zoom_1_9, start_y, 
                                  start_x - zoom_1_3 - zoom_1_9 + sin_table[i] * zoom_1_9, start_y + cos_table[i] * zoom_1_9,
                                  start_x - zoom_1_3 - zoom_1_9 + sin_table[i + 1] * zoom_1_9, start_y + cos_table[i + 1] * zoom_1_9, a);
            }
            if (selected) { state->r.rect(start_x - zoom_2_3 - 3, start_y - zoom_1_6 - 3, start_x + 3, start_y + zoom_1_6 + 3, selection); }
        }
    }
}

void draw_output(State* state, Object* object, Node* nodes, int view_x, int view_y, int spacing_zoom) {
    Node* a = get_node(nodes, object->a);
    const Color* a_color;
    
    if (a->state == 0b00) {
        a_color = &wire_disconnected;
    } else if (a->state == 0b01) {
        a_color = &wire_off;
    } else if (a->state == 0b10) {
        a_color = &wire_on;
    } else if (a->state == 0b11) {
        a_color = &wire_conflicted;
    }

    draw_output_symbol(state, object->x, object->y, object->rotation, view_x, view_y, spacing_zoom, object->selected, *a_color, Color(0, 0, 0));
}

void draw_objects_nodes(State* state, Object* objects, Node* nodes, int view_x, int view_y, int zoom) {
    Object* object = objects;
    while (object != nullptr) {
        switch (object->type) {
        case 'N': // NMOS
            draw_nmos(state, object, nodes, view_x, view_y, zoom);
            break;
        case 'P': // PMOS
            draw_pmos(state, object, nodes, view_x, view_y, zoom);
            break;
        case '+': // V+
            draw_vplus(state, object, nodes, view_x, view_y, zoom);
            break;
        case '-': // V-
            draw_vminus(state, object, nodes, view_x, view_y, zoom);
            break;
        case 'I': // Input
            draw_input(state, object, nodes, view_x, view_y, zoom);
            break;
        case 'O': // Output
            draw_output(state, object, nodes, view_x, view_y, zoom);
            break;
        default:
            break;
        }

        object = object->next;
    }

    Node* node = nodes;
    const Color* node_color;
    Wire* wire;
    while (node != nullptr) {
        if (node->state == 0b00) {
            node_color = &wire_disconnected;
        } else if (node->state == 0b01) {
            node_color = &wire_off;
        } else if (node->state == 0b10) {
            node_color = &wire_on;
        } else if (node->state == 0b11) {
            node_color = &wire_conflicted;
        }

        wire = node->wires;
        while (wire != nullptr) {
            state->r.line(wire->x1 * zoom + view_x, 
                          wire->y1 * zoom + view_y, 
                          wire->x2 * zoom + view_x, 
                          wire->y2 * zoom + view_y, 
                          *node_color);
            if (wire->selected) { state->r.rect(wire->x1 * zoom + view_x - 3, wire->y1 * zoom + view_y - 3, wire->x2 * zoom + view_x + 3, wire->y2 * zoom + view_y + 3, selection); }
            wire = wire->next;
        }

        node = node->next;
    }
}

bool point_in_rect(int x, int y, int x1, int y1, int x2, int y2) {
    if (x2 < x1) {
        int temp_x = x1;
        x1 = x2;
        x2 = temp_x;
    }
    if (y2 < y1) {
        int temp_y = y1;
        y1 = y2;
        y2 = temp_y;
    }
    return x > x1 && x < x2 && y > y1 && y < y2;
}

bool object_in_rect(Object* object, int rect_x1, int rect_y1, int rect_x2, int rect_y2, int view_x, int view_y, int zoom) {
    int x = object->x;
    int y = object->y;
    char rot = object->rotation;
    int x1, y1, x2, y2;
    if (object->type == 'N' || object->type == 'P') {
        x1 = x * zoom + view_x - 3;
        y1 = y * zoom + view_y - 3;
        if (rot == 0 || rot == 2) {
            x2 = x * zoom + (zoom * 2) + view_x + 3;
            y2 = y * zoom + zoom + view_y + 3;
        } else if (rot == 1 || rot == 3) {
            x2 = x * zoom + zoom + view_x + 3;
            y2 = y * zoom + (zoom * 2) + view_y + 3;
        }
    } else if (object->type == '+') {
        if (rot == 0) {
            x1 = x * zoom - zoom * 0.176777 + view_x - 3; // 0.176777 = sqrt(2) / 8
            y1 = y * zoom - zoom * 0.707107 + view_y - 3; // 0.707107 = sqrt(2) / 2
            x2 = x * zoom + zoom * 0.176777 + view_x + 3;
            y2 = y * zoom + view_y + 3;
        } else if (rot == 1) {
            x1 = x * zoom + view_x - 3;
            y1 = y * zoom - zoom * 0.176777 + view_y - 3; // 0.176777 = sqrt(2) / 8
            x2 = x * zoom + zoom * 0.707107 + view_x + 3; // 0.707107 = sqrt(2) / 2
            y2 = y * zoom + zoom * 0.176777 + view_y + 3;
        } else if (rot == 2) {
            x1 = x * zoom - zoom * 0.176777 + view_x - 3; // 0.176777 = sqrt(2) / 8
            y1 = y * zoom + view_y - 3;
            x2 = x * zoom + zoom * 0.176777 + view_x + 3;
            y2 = y * zoom + zoom * 0.707107 + view_y + 3; // 0.707107 = sqrt(2) / 2
        } else if (rot == 3) {
            x1 = x * zoom - zoom * 0.707107 + view_x - 3; // 0.707107 = sqrt(2) / 2
            y1 = y * zoom - zoom * 0.176777 + view_y - 3; // 0.176777 = sqrt(2) / 8
            x2 = x * zoom + view_x + 3;
            y2 = y * zoom + zoom * 0.176777 + view_y + 3;
        }
    } else if (object->type == '-') {
        if (rot == 0) {
            x1 = x * zoom - zoom * 0.176777 + view_x - 3; // 0.176777 = sqrt(2) / 8
            y1 = y * zoom - zoom / 2.0 - zoom * 0.141421 + view_y - 3; // 0.141421 = sqrt(2) / 10
            x2 = x * zoom + zoom * 0.176777 + view_x + 3;
            y2 = y * zoom + view_y + 3;
        } else if (rot == 1) {
            x1 = x * zoom + view_x - 3;
            y1 = y * zoom - zoom * 0.176777 + view_y - 3; // 0.176777 = sqrt(2) / 8
            x2 = x * zoom + zoom / 2.0 + zoom * 0.141421 + view_x + 3; // 0.141421 = sqrt(2) / 10
            y2 = y * zoom + zoom * 0.176777 + view_y + 3;
        } else if (rot == 2) {
            x1 = x * zoom - zoom * 0.176777 + view_x - 3; // 0.176777 = sqrt(2) / 8
            y1 = y * zoom + view_y - 3;
            x2 = x * zoom + zoom * 0.176777 + view_x + 3;
            y2 = y * zoom + zoom / 2.0 + zoom * 0.141421 + view_y + 3; // 0.141421 = sqrt(2) / 10
        } else if (rot == 3) {
            x1 = x * zoom - zoom / 2.0 - zoom * 0.141421 + view_x - 3; // 0.141421 = sqrt(2) / 10
            y1 = y * zoom - zoom * 0.176777 + view_y - 3; // 0.176777 = sqrt(2) / 8
            x2 = x * zoom + view_x + 3;
            y2 = y * zoom + zoom * 0.176777 + view_y + 3;
        }
    } else if (object->type == 'I' || object->type == 'O') {
        if (rot == 0) {
            x1 = x * zoom - zoom / 6.0 + view_x - 3;
            y1 = y * zoom - zoom * 2.0 / 3.0 + view_y - 3;
            x2 = x * zoom + zoom / 6.0 + view_x + 3;
            y2 = y * zoom + view_y + 3;
        } else if (rot == 1) {
            x1 = x * zoom + view_x - 3;
            y1 = y * zoom - zoom / 6.0 + view_y - 3;
            x2 = x * zoom + zoom * 2.0 / 3.0 + view_x + 3;
            y2 = y * zoom + zoom / 6.0 + view_y + 3;
        } else if (rot == 2) {
            x1 = x * zoom - zoom / 6.0 + view_x - 3;
            y1 = y * zoom + view_y - 3;
            x2 = x * zoom + zoom / 6.0 + view_x + 3;
            y2 = y * zoom + zoom * 2.0 / 3.0 + view_y + 3;
        } else if (rot == 3) {
            x1 = x * zoom - zoom * 2.0 / 3.0 + view_x - 3;
            y1 = y * zoom - zoom / 6.0 + view_y - 3;
            x2 = x * zoom + view_x + 3;
            y2 = y * zoom + zoom / 6.0 + view_y + 3;
        }
    }
    return point_in_rect(x1, y1, rect_x1, rect_y1, rect_x2, rect_y2) || 
           point_in_rect(x1, y2, rect_x1, rect_y1, rect_x2, rect_y2) || 
           point_in_rect(x2, y1, rect_x1, rect_y1, rect_x2, rect_y2) || 
           point_in_rect(x2, y2, rect_x1, rect_y1, rect_x2, rect_y2);
}

void update_inputs(State* state, Object* objects, int view_x, int view_y, int zoom) {
    if (state->i.get_mouse_button_pressed(MB_LEFT)) {
        int m_x = state->i.mouse.x;
        int m_y = state->i.mouse.y;
        Object* object = objects;
        while (object != nullptr) {
            if (object->type == 'I') {
                int start_x = object->x * zoom + view_x;
                int start_y = object->y * zoom + view_y;
                int zoom_2_3 = zoom * 2.0 / 3.0;
                int zoom_1_6 = zoom / 6.0;
                if (object->rotation == 0 && point_in_rect(m_x, m_y, start_x - zoom_1_6, start_y - zoom_2_3, start_x + zoom_1_6, start_y)) {
                    object->state = !object->state;
                } else if (object->rotation == 1 && point_in_rect(m_x, m_y, start_x, start_y - zoom_1_6, start_x + zoom_2_3, start_y + zoom_1_6)) {
                    object->state = !object->state;
                } else if (object->rotation == 2 && point_in_rect(m_x, m_y, start_x - zoom_1_6, start_y, start_x + zoom_1_6, start_y + zoom_2_3)) {
                    object->state = !object->state;
                } else if (object->rotation == 3 && point_in_rect(m_x, m_y, start_x - zoom_2_3, start_y - zoom_1_6, start_x, start_y + zoom_1_6)) {
                    object->state = !object->state;
                }
            }
            object = object->next;
        }
    }
}

void select_selection(State* state, Object* objects, Node* nodes, int x1, int y1, int x2, int y2, int view_x, int view_y, int zoom) {
    Object* object = objects;
    while (object != nullptr) {
        object->selected = object_in_rect(object, x1, y1, x2, y2, view_x, view_y, zoom) || (object->selected && state->i.get_mod_key(MKC_SHIFT));
        object = object->next;
    }

    Node* node = nodes;
    Wire* wire;
    while (node != nullptr) {
        wire = node->wires;
        while (wire != nullptr) {
            wire->selected = point_in_rect(wire->x1 * zoom + view_x, wire->y1 * zoom + view_y, x1, y1, x2, y2) || 
                             point_in_rect(wire->x2 * zoom + view_x, wire->y2 * zoom + view_y, x1, y1, x2, y2) ||
                             (wire->selected && state->i.get_mod_key(MKC_SHIFT));
            wire = wire->next;
        }
        node = node->next;
    }
}

void update_draw_selection(State* state, Object* objects, Node* nodes, int* selection_x, int* selection_y, int view_x, int view_y, int zoom) {
    if (state->i.get_mouse_button_pressed(MB_RIGHT)) {
        *selection_x = state->i.mouse.x - view_x;
        *selection_y = state->i.mouse.y - view_y;
    }
    if (state->i.get_mouse_button(MB_RIGHT)) {
        state->r.rect(*selection_x + view_x, *selection_y + view_y, state->i.mouse.x, state->i.mouse.y, selection);
        select_selection(state, objects, nodes, *selection_x + view_x, *selection_y + view_y, state->i.mouse.x, state->i.mouse.y, view_x, view_y, zoom);
    }
}

inline int screenspace_to_gridspace(int n, int view, int zoom) {
    return round((float)(n - view) / (float)zoom);
}

void drag_selection(State* state, Object* objects, Node* nodes, int* drag_x, int* drag_y, int* prev_drag_x, int* prev_drag_y, int m_x, int m_y) {
    if (state->i.get_mouse_button_pressed(MB_LEFT)) {
        *prev_drag_x = m_x;
        *prev_drag_y = m_y;
    }
    
    if (state->i.get_mouse_button(MB_LEFT)) {
        *drag_x = m_x;
        *drag_y = m_y;
        int delta_x = *drag_x - *prev_drag_x;
        int delta_y = *drag_y - *prev_drag_y;
        if (drag_x - prev_drag_x != 0 || drag_y - prev_drag_y != 0) {   
            Object* object = objects;
            while (object != nullptr) {
                if (object->selected) {
                    object->x += delta_x;
                    object->y += delta_y;
                }
                
                object = object->next;
            }

            Node* node = nodes;
            Wire* wire;
            while (node != nullptr) {
                wire = node->wires;
                while (wire != nullptr) {
                    if (wire->selected) {
                        wire->x1 += delta_x;
                        wire->y1 += delta_y;
                        wire->x2 += delta_x;
                        wire->y2 += delta_y;
                    }

                    wire = wire->next;
                }

                node = node->next;
            }
        }

        *prev_drag_x = *drag_x;
        *prev_drag_y = *drag_y;
    }
}

void delete_selection(State* state, Object** objects, Node** nodes) {
    if (state->i.get_key_pressed(KC_BACKSPACE)) {
        Object* object = *objects;
        Object* next_object;
        while (object != nullptr) {
            next_object = object->next;
            
            if (object->selected) {
                delete_object(objects, object);
            }
            object = next_object;
        }

        Node* node = *nodes;
        Node* next_node;
        Wire* wire; 
        Wire* next_wire;
        while (node != nullptr) {
            next_node = node->next;
            
            wire = node->wires;
            while (wire != nullptr) {
                next_wire = wire->next;

                if (wire->selected) {
                    delete_wire(&node->wires, wire);
                }

                wire = next_wire;
            }

            if (node->wires == nullptr) {
                delete_node(nodes, node);
            }

            node = next_node;
        }
    }
}

void place_object(State* state, Object** objects, int m_x, int m_y) {
    if (state->i.get_key_pressed(KC_1)) {
        append_object(objects, {'N', false, m_x, m_y, 0, 0, 0, 0, false, 0, 0});
    } else if (state->i.get_key_pressed(KC_2)) {
        append_object(objects, {'P', false, m_x, m_y, 0, 0, 0, 0, false, 0, 0});
    } else if (state->i.get_key_pressed(KC_3)) {
        append_object(objects, {'+', false, m_x, m_y, 0, 0, 0, 0, false, 0, 0});
    } else if (state->i.get_key_pressed(KC_4)) {
        append_object(objects, {'-', false, m_x, m_y, 0, 0, 0, 0, false, 0, 0});
    } else if (state->i.get_key_pressed(KC_5)) {
        append_object(objects, {'I', false, m_x, m_y, 0, 0, 0, 0, false, 0, 0});
    } else if (state->i.get_key_pressed(KC_6)) {
        append_object(objects, {'O', false, m_x, m_y, 0, 0, 0, 0, false, 0, 0});
    }    
}

void rotate_selection(State* state, Object* objects, Node* nodes, int view_x, int view_y, int zoom) {
    static int center_x = 0;
    static int center_y = 0;
    
    state->r.rect(center_x * zoom + view_x - 3, center_y * zoom + view_y - 3, center_x * zoom + view_x + 3, center_y * zoom + view_y + 3, Color(255, 0, 0, 255));

    if (state->i.get_key(KC_R)) {
        center_x = 0;
        center_y = 0;

        int l_bound_x = 0;
        int l_bound_y = 0;
        int u_bound_x = 0;
        int u_bound_y = 0;

        Object* object = objects;
        Node* node = nodes;
        Wire* wire;
        
        while (object != nullptr) {
            if (object->selected) {
                if (object->x <= l_bound_x) {
                    l_bound_x = object->x;
                } else if (object->x >= u_bound_x) {
                    u_bound_x = object->x;
                }
                if (object->y <= l_bound_y) {
                    l_bound_y = object->y;
                } else if (object->y >= u_bound_y) {
                    u_bound_y = object->y;
                }
            }
            object = object->next;
        }

        while (node != nullptr) {
            wire = node->wires;
            while (wire != nullptr) {
                if (wire->selected) {
                    if (wire->x1 <= l_bound_x) {
                        l_bound_x = wire->x1;
                    } else if (wire->x1 >= u_bound_x) {
                        u_bound_x = wire->x1;
                    }
                    if (wire->y1 <= l_bound_y) {
                        l_bound_y = wire->y1;
                    } else if (wire->y1 >= u_bound_y) {
                        u_bound_y = wire->y1;
                    }
                    if (wire->x2 <= l_bound_x) {
                        l_bound_x = wire->x2;
                    } else if (wire->x2 >= u_bound_x) {
                        u_bound_x = wire->x2;
                    }
                    if (wire->y2 <= l_bound_y) {
                        l_bound_y = wire->y2;
                    } else if (wire->y2 >= u_bound_y) {
                        u_bound_y = wire->y2;
                    }
                }
                wire = wire->next;
            }
            node = node->next;
        }

        center_x = l_bound_x + (u_bound_x - l_bound_x) / 2;
        center_y = l_bound_y + (u_bound_y - l_bound_y) / 2;

        printf("(%i, %i), (%i, %i), (%i, %i)\n", l_bound_x, l_bound_y, u_bound_x, u_bound_y, center_x, center_y);
        if (state->i.get_key_pressed(KC_F)) {
        
        int temp;

        object = objects;
        while (object != nullptr) {
            if (object->selected) {
                object->x -= center_x;
                object->y -= center_y;
                temp = -object->y;
                object->y = object->x;
                object->x = temp;
                object->x += center_x;
                object->y += center_y;
            }
            object = object->next;
        }

        node = nodes;
        while (node != nullptr) {
            wire = node->wires;
            while (wire != nullptr) {
                if (wire->selected) {
                    wire->x1 -= center_x;
                    wire->y1 -= center_x;
                    temp = -wire->y1;
                    wire->y1 = wire->x1;
                    wire->x1 = temp;
                    wire->x1 += center_x;
                    wire->y1 += center_y;
                    
                    wire->x2 -= center_x;
                    wire->y2 -= center_x;
                    temp = -wire->y2;
                    wire->y2 = wire->x2;
                    wire->x2 = temp;
                    wire->x2 += center_x;
                    wire->y2 += center_y;
                }
                wire = wire->next;
            }
            node = node->next;
        }
    }
    }
}

int main() {
    State state = State("test", 640, 480, 120, WINDOW_RESIZEABLE | INPUT_MULTI_THREADED | ELEMENTS_ENABLE);

    const int grid_spacing = 100;

    Object* objects = 0;
    append_object(&objects, {'I', false, 4, 3, 2, 1, 2, 3, false, 0, 0});
    append_object(&objects, {'O', false, 7, 7, 2, 1, 2, 3, false, 0, 0});
    Wire* wires0 = 0;
    append_wire(&wires0, {1, 1, 3, 1, false, 0, 0});
    append_wire(&wires0, {3, 1, 3, 2, false, 0, 0});
    Wire* wires1 = 0;
    append_wire(&wires1, {1, 3, 3, 3, false, 0, 0});
    append_wire(&wires1, {3, 3, 3, 4, false, 0, 0});
    Wire* wires2 = 0;
    append_wire(&wires2, {4, 1, 6, 1, false, 0, 0});
    append_wire(&wires2, {4, 1, 4, 2, false, 0, 0});
    append_wire(&wires2, {6, 1, 6, 2, false, 0, 0});
    Wire* wires3 = 0;
    append_wire(&wires3, {5, 3, 6, 3, false, 0, 0});
    append_wire(&wires3, {6, 3, 6, 4, false, 0, 0});
    Node* nodes = 0;
    append_node(&nodes, {0b00, wires0, 0, 0});
    append_node(&nodes, {0b01, wires1, 0, 0});
    append_node(&nodes, {0b10, wires2, 0, 0});
    append_node(&nodes, {0b11, wires3, 0, 0});
    
    int view_x = 0;
    int view_y = 0;
    float zoom = 1;

    int selection_x = 0;
    int selection_y = 0;

    int prev_drag_x = 0;
    int prev_drag_y = 0;
    int drag_x = 0;
    int drag_y = 0;

    int gs_mouse_x = 0;
    int gs_mouse_y = 0;

    while (!state.quit) {
        state.start_frame();

        gs_mouse_x = screenspace_to_gridspace(state.i.mouse.x, view_x, grid_spacing * zoom);
        gs_mouse_y = screenspace_to_gridspace(state.i.mouse.y, view_y, grid_spacing * zoom);

        if (state.i.get_key_pressed(KC_T)) {
            if (objects[0].type == 'N') { objects[0].type = 'P'; } else
            if (objects[0].type == 'P') { objects[0].type = '+'; } else 
            if (objects[0].type == '+') { objects[0].type = '-'; } else 
            if (objects[0].type == '-') { objects[0].type = 'I'; } else 
            if (objects[0].type == 'I') { objects[0].type = 'O'; } else
            if (objects[0].type == 'O') { objects[0].type = 'N'; }
        }

        if (state.i.get_key_pressed(KC_Q)) {
            zoom /= 2.0;
        }
        if (state.i.get_key_pressed(KC_E)) {
            zoom *= 2.0;
        }
        if (state.i.get_key(KC_W)) {
            view_y += 1;
        }
        if (state.i.get_key(KC_S)) {
            view_y -= 1;
        }
        if (state.i.get_key(KC_A)) {
            view_x += 1;
        }
        if (state.i.get_key(KC_D)) {
            view_x -= 1;
        }
        update_inputs(&state, objects, view_x, view_y, grid_spacing * zoom);
        draw_grid(&state, view_x, view_y, zoom, grid_spacing);
        draw_objects_nodes(&state, objects, nodes, view_x, view_y, grid_spacing * zoom);
        update_draw_selection(&state, objects, nodes, &selection_x, &selection_y, view_x, view_y, grid_spacing * zoom);
        drag_selection(&state, objects, nodes, &drag_x, &drag_y, &prev_drag_x, &prev_drag_y, gs_mouse_x, gs_mouse_y);
        delete_selection(&state, &objects, &nodes);
        place_object(&state, &objects, gs_mouse_x, gs_mouse_y);
        rotate_selection(&state, objects, nodes, view_x, view_y, zoom);

        //printf("%i, %i\n", gs_mouse_x, gs_mouse_y);
        state.r.rect(gs_mouse_x * grid_spacing * zoom + view_x - 5, gs_mouse_y * grid_spacing * zoom + view_y - 5, gs_mouse_x * grid_spacing * zoom + view_x + 5, gs_mouse_y * grid_spacing * zoom + view_y + 5, Color(255, 0, 0, 35));

        //printf("%f\t\t%f\n", state.t.dt, state.t.fps);

        state.end_frame();
    }

    free_objects(objects);
    free_nodes(nodes);

    state.destroy_elements();
}