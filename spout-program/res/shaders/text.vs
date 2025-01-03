#version 430 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec1 texArrayIndex>
layout (location = 1) in vec3 position; // <float col, float row, float texLayer>
const vec2 vertices[] = vec2[](
	vec2(0.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 1.0), 
	vec2(1.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 0.0)
);

out vec2 outPosition;
out vec2 TexCoords;

uniform vec2 imgDims;

void main()
{
	//Vertex is 0 or 1. Marks position along each quad. Position should be the origin of each quad, value 0 through numCols/numRows-1
    vec2 unNormalizedPosition = vec2(vertex.x * 1.0 / imgDims.x + position.x / imgDims.x, vertex.y * 1.0 / imgDims.y + position.y / imgDims.y);
	gl_Position = vec4(unNormalizedPosition.x * 2.0 - 1.0, -(unNormalizedPosition.y * 2.0 - 1.0), 0.0, 1.0);
	TexCoords = vertices[gl_VertexID];
	outPosition = position.xy / imgDims.xy;
}  