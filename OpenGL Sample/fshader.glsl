#version 150
in vec3 color;

out vec4 outputColor;

void main()
{
    // Color
    outputColor = vec4(color, 1.0f);
}
