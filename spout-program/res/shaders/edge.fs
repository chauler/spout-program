#version 430 core
layout(location = 0) out vec4 color;

layout(binding=1) uniform sampler2D inputTexture;

uniform mat4 projection;
uniform ivec2 outputSize;

const float step = 1.0;

float intensity(in vec4 color);
vec3 sobel(in float stepx, in float stepy, in vec2 center);

void main() {
	vec2 uv = outputSize.y - gl_FragCoord.xy / outputSize;
	vec4 sampled = texture(inputTexture, uv);
	color = vec4(sobel(step/outputSize.x, step/outputSize.y, uv), 1.0);
}

vec3 sobel(in float stepx, in float stepy, in vec2 center) {
	float tleft = intensity(texture(inputTexture,center + vec2(-stepx,stepy)));
    float left = intensity(texture(inputTexture,center + vec2(-stepx,0)));
    float bleft = intensity(texture(inputTexture,center + vec2(-stepx,-stepy)));
    float top = intensity(texture(inputTexture,center + vec2(0,stepy)));
    float bottom = intensity(texture(inputTexture,center + vec2(0,-stepy)));
    float tright = intensity(texture(inputTexture,center + vec2(stepx,stepy)));
    float right = intensity(texture(inputTexture,center + vec2(stepx,0)));
    float bright = intensity(texture(inputTexture,center + vec2(stepx,-stepy)));

    float x = tleft + 2.0*left + bleft - tright - 2.0*right - bright;
    float y = -tleft - 2.0*top - tright + bleft + 2.0 * bottom + bright;
    float color = sqrt((x*x) + (y*y));
    return vec3(color,color,color);
}

float intensity(in vec4 color){
	return sqrt((color.x*color.x)+(color.y*color.y)+(color.z*color.z));
}