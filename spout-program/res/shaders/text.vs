#version 430 core
layout (location = 0) in vec2 vertex; // <vec2 pos>
layout (location = 1) in vec2 position; // <float col, float row>
const vec2 vertices[] = vec2[](
	vec2(0.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 1.0), 
	vec2(1.0, 0.0), 
	vec2(0.0, 1.0), 
	vec2(1.0, 0.0)
);

out vec2 normalizedPos;
out vec2 glyphTexCoords;

uniform mat4 projection;
uniform vec2 imgDims;
uniform float charSize;

void main()
{
    gl_Position = projection * vec4(vertex.x + (charSize * position.x), (imgDims.y * charSize) - vertex.y - (charSize * position.y), 0.0, 1.0);
	glyphTexCoords = vertices[gl_VertexID];
	normalizedPos = position.xy / imgDims.xy;
}  