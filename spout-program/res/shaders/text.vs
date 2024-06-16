#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec1 texArrayIndex>
layout (location = 1) in vec3 position; // <vec2 pos, float texLayer>
const vec2 vertices[] = vec2[](
	vec2(0.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 1.0), 
	vec2(1.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 0.0)
);

out vec3 TexCoords;

uniform mat4 projection;
uniform vec2 windowDims;
uniform vec2 imgDims;

void main()
{
	
    gl_Position = projection * vec4(vertex.x + (windowDims.x / imgDims.x * position.x), windowDims.y - (vertex.y + (windowDims.y / imgDims.y * position.y)), 0.0, 1.0);
    TexCoords = vec3(vertices[gl_VertexID], position.z);
}  