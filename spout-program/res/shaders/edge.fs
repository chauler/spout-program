#version 430 core
layout(location = 0) out vec4 color;

layout(binding=1) uniform sampler2D inputTexture;

uniform ivec2 outputSize;
uniform mat3 G[2] = mat3[](
                           mat3( 1.0, 2.0, 1.0,  0.0, 0.0, 0.0,   -1.0, -2.0, -1.0 ),
                           mat3( 1.0, 0.0, -1.0,  2.0, 0.0, -2.0,  1.0, 0.0, -1.0 )
                           );

const float step = 1.0;

#define M_PI 3.1415926535897932384626433832795

float intensity(in vec4 color);
vec3 sobel(in float stepx, in float stepy, in vec2 center);

void main() {
	vec2 uv = outputSize.y - gl_FragCoord.xy / outputSize;
	vec4 sampled = texture(inputTexture, uv);
	color = vec4(sobel(step/outputSize.x, step/outputSize.y, gl_FragCoord.xy), 1.0);
}

vec3 sobel(in float stepx, in float stepy, in vec2 center) {
    //mat3 I;
    //float cnv[2];
    //vec3 pixelColor = vec3(0.0);
    //for(int i=0; i<3; i++) {
    //    for(int j=0; j<3; j++) {
    //        pixelColor = texelFetch(inputTexture, ivec2(gl_FragCoord.xy) + ivec2(i-1, j-1), 0).rgb;
    //        I[i][j] = length(pixelColor);
    //    }
    //}
    //
    //for(int i=0; i<2; i++) {
    //    float dp3 = dot(G[i][0], I[0]) + dot(G[i][1], I[1]) + dot(G[i][2], I[2]);
    //    cnv[i] = dp3 * dp3;
    //}
    //return vec4(0.5 * sqrt(cnv[0] * cnv[0] + cnv[1] * cnv[1])).xyz;

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
    //float color = sqrt((x*x) + (y*y));
    //if(abs(x) < 0.1) {
    //    x = 0.;
    //}
    //if(abs(y) < 0.1) {
    //    y = 0.;
    //}
    float angle = atan(y, x);
    angle = angle / M_PI * 0.5 + 0.5;
    //return vec3(x,y,0.);
    return vec3(x,y,angle);
}

float intensity(in vec4 color){
	return length(color);//sqrt((color.x*color.x)+(color.y*color.y)+(color.z*color.z));
}