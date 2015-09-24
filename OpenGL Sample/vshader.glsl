#version 150
in vec3 position;
in vec3 vcolor;

uniform float left;
uniform float top;

out vec3 color;

void main()
{
	vec3 offset = vec3(left, top, 0.0f);
	gl_Position = vec4(position + offset, 1.0f);
	color = vcolor;
}
