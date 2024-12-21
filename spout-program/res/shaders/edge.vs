#version 430 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 uv>
const vec2 vertices[] = vec2[](
	vec2(0.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 1.0), 
	vec2(1.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 0.0)
);

void main()
{
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
}  