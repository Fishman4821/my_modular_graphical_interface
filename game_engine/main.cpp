#include <iostream>
#include <chrono>
#include <thread>

#define SDL_MAIN_HANDLED
#include "SDL2\SDL.h"

#include "common.cpp"
#include "sdl.cpp"

using namespace std;

const chrono::duration<int64_t, std::milli> mspf = chrono::duration<int64_t, std::milli>((1/30) * 1000);

int main() {
	Mesh obj = Mesh("car.obj");

	sdl::init(640, 480);
	sdl::fill(color(0, 0, 0));
	sdl::stroke(color(255, 255, 255));

	Transform* cube_t = (Transform*)malloc(sizeof(Transform));
	cube_t->position = Vec3(200, 0, 0);
	cube_t->rotation = Vec3(0, 0, 0);
	cube_t->scale = Vec3(1, 1, 1);

	Vec3* verts1 = (Vec3*)malloc(sizeof(Vec3) * 8);
	verts1[0] = Vec3(-50, 50, -50);
  	verts1[1] = Vec3(-50, -50, -50);
  	verts1[2] = Vec3(50, -50, -50);
  	verts1[3] = Vec3(50, 50, -50);
  	verts1[4] = Vec3(-50, 50, 50);
  	verts1[5] = Vec3(-50, -50, 50);
  	verts1[6] = Vec3(50, -50, 50);
  	verts1[7] = Vec3(50, 50, 50);

	Vec2* uv_map1 = (Vec2*)malloc(sizeof(Vec2) * 8);
	uv_map1[0] = Vec2(0.35, 0.35);
	uv_map1[1] = Vec2(0.25, 0.5);
	uv_map1[2] = Vec2(0.25, 0.5);
	uv_map1[3] = Vec2(0.25, 0.5);
	uv_map1[4] = Vec2(0.25, 0.5);
	uv_map1[5] = Vec2(0.25, 0.5);
	uv_map1[6] = Vec2(0.25, 0.5);
	uv_map1[7] = Vec2(0.25, 0.5);
	
	SDL_Texture* texture = SDL_CreateTexture(sdl::sdl_objects.renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32, SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC, 8, 1);
	int* pixels = (int*)malloc(32);
	pixels[0] = color(255, 0, 0).bytes();
	pixels[1] = color(0, 255, 0).bytes();
	pixels[2] = color(0, 0, 255).bytes();
	pixels[3] = color(255, 255, 0).bytes();
	pixels[4] = color(0, 255, 255).bytes();
	pixels[5] = color(255, 0, 255).bytes();
	pixels[6] = color(255, 0, 0).bytes();
	pixels[7] = color(255, 255, 255).bytes();

	SDL_UpdateTexture(texture, NULL, pixels, 32);

	Vec3_Int* v_map1 = (Vec3_Int*)malloc(sizeof(Vec3_Int) * 12);
	v_map1[ 0] = Vec3_Int(5, 1, 2);
	v_map1[ 1] = Vec3_Int(2, 6, 5);
	v_map1[ 2] = Vec3_Int(0, 1, 2);
	v_map1[ 3] = Vec3_Int(2, 3, 0);
	v_map1[ 4] = Vec3_Int(0, 1, 5);
	v_map1[ 5] = Vec3_Int(5, 4, 0);
	v_map1[ 6] = Vec3_Int(4, 0, 3);
	v_map1[ 7] = Vec3_Int(3, 7, 4);
	v_map1[ 8] = Vec3_Int(2, 3, 7);
	v_map1[ 9] = Vec3_Int(7, 6, 2);
	v_map1[10] = Vec3_Int(4, 5, 6);
	v_map1[11] = Vec3_Int(6, 7, 4);

	Mesh* cube = (Mesh*)malloc(sizeof(Mesh));
	cube[0] = Mesh(verts1, 8, v_map1, 12, uv_map1, texture);

	Camera* c = (Camera*)malloc(sizeof(Camera));
	c->t = Transform();
	c->t.position = Vec3(0, 0, -200);
	c->t.rotation = Vec3(0, 0, 0);
	c->resolution = Vec2(640, 480);
	c->n = 100;
	c->f = 2000;
	c->set_fov(70);

	free(verts1);
	free(v_map1);
	free(uv_map1);

	Vec3* verts2 = (Vec3*)malloc(sizeof(Vec3) * 6);
	verts2[0] = Vec3(0, 50, 0);
	verts2[1] = Vec3(50, 0, 50);
	verts2[2] = Vec3(-50, 0, 50);
	verts2[3] = Vec3(50, 0, -50);
	verts2[4] = Vec3(-50, 0, -50);
	verts2[5] = Vec3(0, -50, 0);

	Vec3_Int* v_map2 = (Vec3_Int*)malloc(sizeof(Vec3_Int) * 8);
	v_map2[0] = Vec3_Int(0, 1, 2);
	v_map2[1] = Vec3_Int(0, 2, 3);
	v_map2[2] = Vec3_Int(0, 3, 4);
	v_map2[3] = Vec3_Int(0, 4, 1);
	v_map2[4] = Vec3_Int(5, 1, 2);
	v_map2[5] = Vec3_Int(5, 2, 3);
	v_map2[6] = Vec3_Int(5, 3, 4);
	v_map2[7] = Vec3_Int(5, 4, 1);

	Vec2* uv_map2 = (Vec2*)malloc(sizeof(Vec2) * 6);
	uv_map2[0] = Vec2(0.35, 0.35);
	uv_map2[1] = Vec2(0.25, 0.5);
	uv_map2[2] = Vec2(0.25, 0.5);
	uv_map2[3] = Vec2(0.25, 0.5);
	uv_map2[4] = Vec2(0.25, 0.5);
	uv_map2[5] = Vec2(0.25, 0.5);

	Mesh* octohedron = (Mesh*)malloc(sizeof(Mesh));
	octohedron[0] = Mesh(verts2, 6, v_map2, 8, uv_map2, texture);

	free(verts2);
	free(v_map2);
	free(uv_map2);

	Transform* octohedron_t = (Transform*)malloc(sizeof(Transform));
	octohedron_t->position = Vec3(-200, 100, 0);
	octohedron_t->rotation = Vec3(0, 45, 0);
	octohedron_t->scale = Vec3(1, 1, 1);

	sdl::project(Vec3(50, 50, 50), c);

	auto t1 = chrono::high_resolution_clock::now();
	auto t2 = chrono::high_resolution_clock::now();
	while (!quit) {
		t1 = chrono::high_resolution_clock::now();
		sdl::sdl_event_function();
		c->updateCamera();
		sdl::background(color(100, 100, 200));
		sdl::draw_mesh(c, cube_t, cube);
		sdl::draw_mesh(c, octohedron_t, octohedron);
		cube_t->rotation.x += 0.05;
		if (cube_t->rotation.x == 360) {
			cube_t->rotation.x = 0;
		} 
		cube_t->rotation.y += 0.06;
		if (cube_t->rotation.y == 360) {
			cube_t->rotation.y = 0;
		} 
		cube_t->rotation.z += 0.07;
		if (cube_t->rotation.z == 360) {
			cube_t->rotation.z = 0;
		} 
		SDL_RenderPresent(sdl::sdl_objects.renderer);
		t2 = chrono::high_resolution_clock::now();
		this_thread::sleep_for(chrono::nanoseconds(chrono::duration_cast<chrono::nanoseconds>(mspf - (t2 - t1))));
	}

	cube->destruct();
	octohedron->destruct();
	free(pixels);
	free(c);
	free(cube_t);
	free(octohedron_t);
	free(cube);
	free(octohedron);
}