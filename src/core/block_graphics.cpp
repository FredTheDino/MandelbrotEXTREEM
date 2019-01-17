Context initalize_graphics(const char *path)
{
	Context gfx;

	const f32 w = 1.0f;
	f32 square[] = {
		-w, -w,
		 w, -w,
		 w,  w,
		-w,  w
	};

	glGenVertexArrays(1, &gfx.vertex_array);
	glBindVertexArray(gfx.vertex_array);

	glGenBuffers(1, &gfx.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, gfx.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, gfx.vertex_buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

	glEnable(GL_MULTISAMPLE);
	glGenTextures(1, &gfx.tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gfx.tex);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, game.width, game.height, true);

	glGenFramebuffers(1, &gfx.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gfx.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, gfx.tex, 0);

	//glCheckFramebufferStatus(target); // TODO: Assert something here?

	gfx.shader_path = path;
	gfx.shader = compile_progam(path);
	gfx.shader_time = file_timestamp(path);
	return gfx;
}

void destroy_graphics(Context *gfx)
{
	glDeleteVertexArrays(1, &gfx->vertex_array);
	glDeleteProgram(gfx->vertex_buffer);
}

void frame(Context *gfx, Camera *camera)
{
	static u64 counter = 0;

	glBindFramebuffer(GL_FRAMEBUFFER, gfx->fbo);
	glClearColor(0.3f, 0.1f, 0.2f, 1.0f);

	// Recompile shader
	u64 new_time = file_timestamp(gfx->shader_path);
	if (new_time != gfx->shader_time)
	{
		counter = 0;
		gfx->shader_time = new_time;
	}

	if (counter < 30)
	{
		counter++;
		if (counter == 30)
		{
			u32 shader = compile_progam(gfx->shader_path);
			if (shader != (u32) -1)
			{
				gfx->shader = shader;
			}
		}
	}

	Vec2 min = camera->position + (V2(1, game.aspect_ratio)) * camera->zoom;
	Vec2 max = camera->position - (V2(1, game.aspect_ratio)) * camera->zoom;

	GLuint program = gfx->shader;
	glUseProgram(program);
#define u_loc(attr) (glGetUniformLocation(program, attr))
	glUniform1f(u_loc("time"), game.clock.time);
	glUniform2f(u_loc("minimum"), min.x, min.y);
	glUniform2f(u_loc("maximum"), max.x, max.y);
	glUniform2i(u_loc("dimensions"), game.width, game.height);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_QUADS, 0, 4);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gfx->fbo);

	//glDrawBuffer(GL_BACK);

	glBlitFramebuffer(
			0, 0, game.width, game.height, 
			0, 0, game.width, game.height, 
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

#define COMPILATION_FAILED (-1)
u32 compile_progam(const char *file)
{
	const char *source = read_entire_file(file, 0);
	if (!source)
	{
		return COMPILATION_FAILED;
	}

	const char *preamble = "#version 130\n";
#define NUM_SHADER_PARTS 3
	const char *vert_source[NUM_SHADER_PARTS] = {preamble, "#define VERT\n", source};
	const char *frag_source[NUM_SHADER_PARTS] = {preamble, "#define FRAG\n", source};

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vert_shader, NUM_SHADER_PARTS, vert_source, NULL);
	glShaderSource(frag_shader, NUM_SHADER_PARTS, frag_source, NULL);
	pop_memory((void *) source);

#undef NUM_SHADER_PARTS

	glCompileShader(vert_shader);
	{
		GLuint shader = vert_shader;
		GLint is_compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
		if(is_compiled == GL_FALSE)
		{
			GLint max_length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

			// The max_length includes the NULL character
			GLchar *log = (GLchar *) push_memory(sizeof(char) * max_length);
			glGetShaderInfoLog(shader, max_length, &max_length, log);
			print("|| (VERT) GLSL %s\n", log);
			pop_memory(log);

			glDeleteShader(frag_shader);
			glDeleteShader(vert_shader);
			return COMPILATION_FAILED;
		}
	}

	glCompileShader(frag_shader);
	{
		GLuint shader = frag_shader;
		GLint is_compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
		if(is_compiled == GL_FALSE)
		{
			GLint max_length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

			// The max_length includes the NULL character
			GLchar *log = (GLchar *) push_memory(sizeof(GLchar) * max_length);
			glGetShaderInfoLog(shader, max_length, &max_length, log);
			print("|| (FRAG) GLSL %s\n", log);
			pop_memory(log);

			glDeleteShader(frag_shader);
			glDeleteShader(vert_shader);
			return COMPILATION_FAILED;
		}
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);
	glDetachShader(program, vert_shader);
	glDetachShader(program, frag_shader);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return program;
}

#undef COMPILATION_FAILED


