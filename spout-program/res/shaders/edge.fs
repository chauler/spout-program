#version 430 core
layout(location = 0) out vec4 color;

layout(binding=0) uniform sampler2D inputTexture;

uniform ivec2 outputSize;
uniform mat3 G[2] = mat3[](
                           mat3( 1.0, 2.0, 1.0,  0.0, 0.0, 0.0,   -1.0, -2.0, -1.0 ),
                           mat3( 1.0, 0.0, -1.0,  2.0, 0.0, -2.0,  1.0, 0.0, -1.0 )
                           );

const float step = 1.0;

#define M_PI 3.1415926535897932384626433832795

float intensity(in vec4 color);
vec2 sobel(in float stepx, in float stepy, in vec2 center);

void main() {
	vec2 uv = outputSize.y - gl_FragCoord.xy / outputSize;
	vec4 sampled = texture(inputTexture, uv);
    vec2 edges = sobel(step/outputSize.x, step/outputSize.y, gl_FragCoord.xy);
    color = vec4(1.0);
    if(edges.y >= 0.05 && edges.y <= 0.2 || edges.y >= 0.55 && edges.y <= 0.7) {
        color = vec4(0.0, 0.0, 1.0, 1.0); //blue
    }
    if(edges.y < 0.05 || edges.y > 0.95 || edges.y > 0.45 && edges.y < 0.55) { //vertical lines
        color = vec4(1.0, 0.0, 0.0, 1.0); //red
    }
    else if(edges.y > 0.2 && edges.y < 0.3 || edges.y > 0.7 && edges.y < 0.8) { //horizontal lines
        color = vec4(1.0, 1.0, 0.0, 1.0); //yellow
    }
    else if(edges.y >= 0.3 && edges.y <= 0.45 || edges.y >= 0.8 && edges.y <= 0.95) { // /
        color = vec4(0.0, 1.0, 0.0, 1.0); //green
    }
    else if(edges.y >= 0.05 && edges.y <= 0.2 || edges.y >= 0.55 && edges.y <= 0.7) { // \
        color = vec4(0.0, 0.0, 1.0, 1.0); //blue
    }
    if(edges.x < 0.1) {
        color = vec4(0.0);
    }
}

vec2 sobel(in float stepx, in float stepy, in vec2 center) {
	float tleft = intensity(texelFetch( inputTexture,ivec2(center + vec2(-1,1)),  0));
    float left = intensity(texelFetch(  inputTexture,ivec2(center + vec2(-1,0)),  0));
    float bleft = intensity(texelFetch( inputTexture,ivec2(center + vec2(-1,-1)), 0));
    float top = intensity(texelFetch(   inputTexture,ivec2(center + vec2(0,1)),   0));
    float bottom = intensity(texelFetch(inputTexture,ivec2(center + vec2(0,-1)),  0));
    float tright = intensity(texelFetch(inputTexture,ivec2(center + vec2(1,1)),   0));
    float right = intensity(texelFetch( inputTexture,ivec2(center + vec2(1,0)),   0));
    float bright = intensity(texelFetch(inputTexture,ivec2(center + vec2(1,-1)),  0));
    
    float x = tleft  + 2.0*left + bleft - tright - 2.0*right - bright;
    float y = -left  - 2.0*top - tright + bleft + 2.0*bottom + bright;

    float angle = atan(y, x);
    angle = angle / M_PI * 0.5 + 0.5; //convert to range of 0-1
    float val = length(vec2(x, y));
    return vec2(val, angle);
}

float intensity(in vec4 color){
	return length(color);
}