// Transform
uniform float time;
uniform vec2 minimum;
uniform vec2 maximum;

uniform ivec2 dimensions;

#ifdef VERT

in vec2 in_position;

void main()
{
	gl_Position = vec4(in_position.xy, 0, 1);
}

#else

out vec4 out_color;

void main()
{
	float x = gl_FragCoord.x / dimensions.x;
	float y = gl_FragCoord.y / dimensions.y;
	float nx = float(minimum.x * (1.0 - x) + maximum.x * x);
	float ny = float(minimum.y * (1.0 - y) + maximum.y * y);

	float zx = 0;
	float zy = 0;
	out_color = vec4(0, 0, 0, 0);
	for (int i = 0; i < int(sin(time) * 400.0 + 400.0); i++)
	{
		float x = zx * zx - zy * zy + nx;
		float y = zy * zx + zy * zx + ny;
		if (x * x + y * y > 4)
		{
			out_color = vec4(i * 0.001, i * 0.005, i * 0.01, 1.0);
			break;
		}
		zx = x;
		zy = y;
	}
}

#endif

