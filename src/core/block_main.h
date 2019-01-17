// THIS FILE
// contains the entry point for the program, "run()", and
// ties all of the sub modules together.
#ifndef __BLOCK_MAIN__
#define __BLOCK_MAIN__

// Types, you should preferably use these types since you
// don't know how large the actual C/C++ types are.
#include <stdint.h>
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64; // We assume these are a thing.
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64; // We assume these are a thing.
typedef float f32;
typedef double f64;

#include "../math/block_math.h"

bool str_eq(const char *_a, const char *_b, u32 len=-1)
{
	u32 i = 0;
	char *a = (char *) _a;
	char *b = (char *) _b;
	while (i < len)
	{
		if (!*a || !*b) break;
		if (*(a++) != *(b++)) return false;
		i++;
	}
	return true;
}

// the file was last edited.
u64 (*file_timestamp)(const char *file);
// Copy the whole file to memory.
const char *(*read_entire_file)(const char *file, u64 *file_size);

#define HALT_AND_CATCH_FIRE() (((int*) 0)[0] = 0)

#define ASSERT(expr) {if (!(expr)) {\
	print("ASSERT FAILED: %s:%d\n", __FILE__, __LINE__); HALT_AND_CATCH_FIRE(); }}

#define MS_TO_SEC(s) ((double) (s) / 1000.0)
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

struct Clock
{
	f64 delta;
	f64 time;
};

// Returns the current tick, this is used to feed the game clock.
f64 get_time();

// Update the mani clock in the game.
void update_game_clock(Clock *clock);

// Forward declarations of important stuff.
struct 
{
	// TODO: Rendering resolution shouldn't have to be 
	// the same as the window.
	SDL_Window *window;
	s32 width, height;
	f32 aspect_ratio;

	SDL_GLContext gl_context;

	bool running;

	Clock clock;

} game;

//
// Window handeling fucntions.
//

// This sends an SDL event through the system, 
// calling "set_viewport_size".
void set_window_size(s32 width, s32 height)
{
	SDL_SetWindowSize(game.window, width, height);
}

// Updates the global "game" state with the dimensions.
void set_viewport_size(s32 width, s32 height)
{
	glViewport(0, 0, width, height);
	game.width = width;
	game.height = height;
	game.aspect_ratio = (f32) height / width;
}

// Start up and initalize all the libraries.
void initalize_libraries();
// Free the ememory and close the libds.
void destroy_libraries();

// Run the actuall game, should be called from the platform layer.
void run();

// Count the FPS for the last second.
void count_fps()
{
	static f32 clock = 0.0;
	static u64 frames = 0;
	clock += game.clock.delta;
	frames++;
	while (clock > 1.0f)
	{
		print("FPS: %d\n", frames);
		clock -= 1.0f;
		frames = 0;
	}
}

#endif
