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

out vec2 normalizedPos;
out vec3 glyphTexCoords;

uniform mat4 projection;
uniform vec2 windowDims;
uniform vec2 imgDims;
uniform float charSize;

void main()
{
    //gl_Position = projection * vec4(vertex.x + (windowDims.x / imgDims.x * position.x), windowDims.y - (vertex.y + (windowDims.y / imgDims.y * position.y)), 0.0, 1.0);
    gl_Position = projection * vec4(vertex.x + (charSize * position.x), (imgDims.y * charSize) - vertex.y - (charSize * position.y), 0.0, 1.0);
	glyphTexCoords = vec3(vertices[gl_VertexID], position.z);
	normalizedPos = position.xy / imgDims.xy;
}  