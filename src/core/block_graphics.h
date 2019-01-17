#ifndef __BLOCK_GRAPHICS__
#define __BLOCK_GRAPHICS__
//
// Note:
// There are cirtain variables in the shader
// that the is assumed to be there. If they 
// are named incorrectly or not pressent, the
// rendering will look wierd.
//

struct Camera
{
	// General transform
	Vec2 position;
	f32 zoom;
	f32 rotation;
};

struct Context
{
	GLuint vertex_array;
	GLuint vertex_buffer;

	const char *shader_path;
	u64 shader_time;
	u32 shader;

	u32 fbo;
	u32 tex;
};

// 
// Shaders
//

// Create the shader program asset
u32 compile_progam(const char *file);

#endif
