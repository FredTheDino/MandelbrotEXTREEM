#ifdef WIN32
#pragma warning(push, 0)
#endif

bool running = true;

// SDL
#include <SDL2/SDL.h>

// OpenGL
#include <glad/glad.h>
#include <glad/glad.c>

#ifdef WIN32
#pragma warning(pop)
#endif

#include "block_main.h"
#include "block_memory.h"
#include "block_memory.cpp"

#include "block_list.h"

#include "block_input.h"
#include "block_input.cpp"

#include "block_graphics.h"
#include "block_graphics.cpp"

void initalize_libraries()
{
	auto error = SDL_Init(SDL_INIT_EVERYTHING);
	ASSERT(error == 0);

	s32 window_width = 800;
	s32 window_height = 800;
	bool vsync = true;

	game.window = SDL_CreateWindow("Mandelbrot Interactive Remasterd (Extreem)", 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	ASSERT(game.window);

	game.gl_context = SDL_GL_CreateContext(game.window);
	SDL_GL_SetSwapInterval(vsync);
	ASSERT(game.gl_context);

	ASSERT(gladLoadGL());

	// Perhaps make this optional. 
	print("|| OpenGL Version: %d.%d\n", GLVersion.major, GLVersion.minor);

	set_viewport_size(window_width, window_height);
}

void destroy_libraries()
{
	SDL_Quit();
}

inline
f64 get_time()
{
	return MS_TO_SEC(SDL_GetTicks());
}

void update_game_clock(Clock *clock)
{
	f64 current_time = get_time();
	clock->delta = current_time - clock->time;
	clock->time = current_time;
}

void run()
{
	initalize_libraries();

	Context gfx = initalize_graphics("res/shader.glsl");
	InputManager manager = initalize_input();

	const char *RESET = "reset";
	const char *VERT = "vert";
	const char *HORI = "hori";
	const char *ZOOM = "zoom";
	const char *CLOSE = "close";
	add_input(manager, VERT);
	add_binding(manager, KEY(w), VERT,  1);
	add_binding(manager, KEY(s), VERT, -1);
	add_binding(manager, CBUTTON(DPAD_UP), VERT,  1);
	add_binding(manager, CBUTTON(DPAD_DOWN), VERT, -1);
	add_binding(manager, CAXIS(LEFTY), VERT);
	add_input(manager, HORI);
	add_binding(manager, KEY(d), HORI,  1);
	add_binding(manager, KEY(a), HORI, -1);
	add_binding(manager, CBUTTON(DPAD_RIGHT), HORI,  1);
	add_binding(manager, CBUTTON(DPAD_LEFT), HORI, -1);
	add_binding(manager, CAXIS(LEFTX), VERT);
	add_input(manager, ZOOM);
	add_binding(manager, KEY(q), ZOOM,  1);
	add_binding(manager, KEY(e), ZOOM, -1);
	add_binding(manager, CAXIS(RIGHTY), ZOOM);
	add_input(manager, RESET);
	add_binding(manager, KEY(r), RESET);
	add_binding(manager, CBUTTON(X), RESET);
	add_input(manager, CLOSE);
	add_binding(manager, KEY(ESCAPE), CLOSE);

	Camera camera = {};
	camera.zoom = 2.5f;
	
	const f32 zoom_speed = 2;
	const f32 speed = 1;
	while (!released(manager, CLOSE) && running)
	{
		update_game_clock(&game.clock);
		f32 delta = game.clock.delta;
		
		if (pressed(manager, RESET))
		{
			camera = {};
			camera.zoom = 2.5f;
		}

		update_input(&manager);
		camera.zoom *= (1 + zoom_speed * value(manager, ZOOM) * delta);
		camera.position.x -= speed * value(manager, HORI) * delta * camera.zoom;
		camera.position.y -= speed * value(manager, VERT) * delta * camera.zoom;


		frame(&gfx, &camera);
		SDL_GL_SwapWindow(game.window);
	}

	// #LetTheOSDoIt.
	destroy_libraries();
}

