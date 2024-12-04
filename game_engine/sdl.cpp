#pragma once

#include <iostream>

#include "common.cpp"
#include "SDL.h"
#include "quicksort.cpp"

namespace sdl {
	struct {
		SDL_Window *window;
		SDL_Surface *window_surface;
		SDL_Renderer *renderer;
	} sdl_objects;

	float time;

	void sdl_event_function() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true; 
			}
			break;
		}
	}
	

	void init(int width, int height) {
		SDL_Init(SDL_INIT_VIDEO);
		sdl_objects.window = SDL_CreateWindow(
								windowTitle,
								SDL_WINDOWPOS_CENTERED,
								SDL_WINDOWPOS_CENTERED,
								width, height,
								0);

		sdl_objects.renderer = SDL_CreateRenderer(sdl_objects.window, -1, 0);
	} 

	color fill_color;
	color stroke_color;

	void triangle(Vec2 a, Vec2 b, Vec2 c, Vec2 a_uv, Vec2 b_uv, Vec2 c_uv, SDL_Texture* t) {
		SDL_Vertex vertices[] = {
		{{a.x, a.y}, {fill_color.r, fill_color.g, fill_color.b, fill_color.a}, {a_uv.x, a_uv.y}},
		{{b.x, b.y}, {fill_color.r, fill_color.g, fill_color.b, fill_color.a}, {b_uv.x, b_uv.y}},
		{{c.x, c.y}, {fill_color.r, fill_color.g, fill_color.b, fill_color.a}, {c_uv.x, c_uv.y}}};
		SDL_RenderGeometry(sdl_objects.renderer, t, vertices, 3, NULL, 0);

		SDL_SetRenderDrawColor(sdl_objects.renderer, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
		SDL_RenderDrawLine(sdl_objects.renderer, a.x, a.y, b.x, b.y);
		SDL_RenderDrawLine(sdl_objects.renderer, b.x, b.y, c.x, c.y);
		SDL_RenderDrawLine(sdl_objects.renderer, c.x, c.y, a.x, a.y);
	}
	
	void fill(color rgb) {
		fill_color = rgb;
	}

	void background(color rgb) {
		SDL_SetRenderDrawColor(sdl_objects.renderer, rgb.r, rgb.g, rgb.b, 255); 
		SDL_RenderClear(sdl_objects.renderer);
	}

	void stroke(color rgb) {
		stroke_color = rgb;
	}

	Vec2 project(Vec3 a, Camera* c) {
		Vec3 d = (a - c[0].t.position).rotate(c[0].t.rotation.negate());

		Vec4 clip = Vec4(
			c->n / c->r_project * d.x,
			c->n / c->t_project * d.y,
			-d.z + -2 * c->n,
			-d.z
		);
		//printf("%f, %f, %f, %f, %f, %f\n", d.x, d.y, d.z, clip.z, c->resolution.x / 2 * (clip.x / clip.w) + c->resolution.x / 2, c->resolution.y / 2 * (clip.y / clip.w) + c->resolution.y / 2);
		return Vec2(c->resolution.x / 2 * (clip.x / clip.w) + c->resolution.x / 2, c->resolution.y / 2 * (clip.y / clip.w) + c->resolution.y / 2);
		
	}

	void draw_mesh(Camera* c, Transform* t, Mesh* mesh) {
		Vec3* t_verts = (Vec3*)malloc(sizeof(Vec3) * mesh->vert_count);
		memset(t_verts, 0, sizeof(Vec3) * mesh->vert_count);

		for (int i = 0; i < mesh->vert_count; i++) {
			t_verts[i] = ((mesh->verts[i].rotate(t->rotation) + t->position) * t->scale);
		}

		quicksorts::quickSort_v_map(mesh->v_map, mesh->v_map_length, t_verts, mesh->vert_count, c[0].t.position, c[0].t.rotation);

		Vec3_Int map = Vec3_Int(0, 0, 0);
		for (int map_index = 0; map_index < mesh->v_map_length; map_index++) {
			map = mesh->v_map[map_index];

			// cout << project(t_verts[map.x], c).x << "\n";
			// cout << project(t_verts[map.x], c).y << "\n";

			triangle(project(t_verts[map.x], c), 
					 project(t_verts[map.y], c), 
					 project(t_verts[map.z], c),
					 mesh->uv_map[map.x],
					 mesh->uv_map[map.y],
					 mesh->uv_map[map.z],
					 mesh->t);
		}

		free(t_verts);
	}
};