#version 430 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec1 texArrayIndex>
layout (location = 1) in vec3 position; // <float col, float row, float texLayer>
layout (location = 2) in vec3 inColor;
const vec2 vertices[] = vec2[](
	vec2(0.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 1.0), 
	vec2(1.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 0.0)
);

out vec2 outPosition;
out vec3 TexCoords;
out vec3 pixelColor;

uniform vec2 imgDims;

void main()
{
    vec2 unNormalizedPosition = vec2(vertex.x * 1.0 / imgDims.x + position.x / imgDims.x, vertex.y * 1.0 / imgDims.y + position.y / imgDims.y);
	gl_Position = vec4(unNormalizedPosition.x * 2.0 - 1.0, -(unNormalizedPosition.y * 2.0 - 1.0), 0.0, 1.0);
	TexCoords = vec3(vertices[gl_VertexID], position.z);
	pixelColor = vec3(vertices[gl_VertexID], 0.0);
	outPosition = position.xy / imgDims.xy;
}  