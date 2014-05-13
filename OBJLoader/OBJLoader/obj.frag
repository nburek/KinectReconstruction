#version 400

in vec3 color;
layout (location = 0) out vec4 fragment_color;

void main(void)
{
		fragment_color = vec4(color, 1.0);
}
