#version 430 core
layout (location = 0) in vec3 vertex; // <vec3 pos>
layout (location = 2) in vec2 TexCoord;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(vertex, 1.0);
	TexCoords = TexCoord;
}