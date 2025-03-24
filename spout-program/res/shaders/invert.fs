#version 430 core
layout(location = 0) out vec4 color;

layout(binding=0) uniform sampler2D inputTexture;

in vec2 TexCoords;

void main() {
	vec4 colorSample = texture(inputTexture, vec2(TexCoords.x, 1-TexCoords.y));
	color = colorSample;
}