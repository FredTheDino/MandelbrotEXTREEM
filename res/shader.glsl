// Transform
uniform float time;
uniform int depth;
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

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

#if 0
vec4 color_for(float i)
{
	vec4 colors[5]; 
	colors[0] = vec4(0, 0, 0.5, 1.0);
	colors[1] = vec4(0, 0.5, 1.0, 1.0);
	colors[2] = vec4(1.0, 1.0, 1.0, 1.0);
	colors[3] = vec4(1.0, 0.64, 0.0, 1.0);
	colors[4] = vec4(0, 0, 0.125, 1.0);

	float i2 = sqrt(i) * 10;
	float grad_length = 90.0;
	int step_length = int(grad_length) / 5;

	float step_i
	return colors[0];
}
#endif

void main()
{
	const float log_2 = log(2.0);

	float x = gl_FragCoord.x / dimensions.x;
	float y = gl_FragCoord.y / dimensions.y;
	float nx = float(minimum.x * (1.0 - x) + maximum.x * x);
	float ny = float(minimum.y * (1.0 - y) + maximum.y * y);

	float cx = 0.285;
	float cy = 0.0;
	cx = nx;
	cy = ny;

	float zx = 0;
	float zy = 0;
	out_color = vec4(0, 0, 0, 0);
	for (int i = 0; i < depth; i++)
	{
		float x = zx * zx - zy * zy - cx;
		float y = zy * zx + zy * zx - cy;
		if (x * x + y * y > 1<<16)
		{
			float log_zn = log(x*x + y*y) / 2;
			float nu = log(log_zn / log_2) / log_2; 
			float i2 = i + 1 - nu;
			out_color = vec4(i2 * 1.1, i2 * i2 * 1.1, i2 * 0.01, 1.0);
			// out_color = vec4(hsv2rgb(vec3(i2 / 32.0 + time, i2 * 0.02, 0.5)), 1.0);
			break;
		}
		zx = x;
		zy = y;
	}
}

#endif

