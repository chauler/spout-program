#version 430 core
in vec3 TexCoords;
in vec3 pixelColor;
in vec2 outPosition;
layout(location = 0) out vec4 color;

//uniform sampler2D text;
layout(binding=0) uniform sampler2DArray textArray;
layout(binding=1) uniform sampler2D inputTexture;
uniform vec4 charColor;
uniform vec4 bgColor;

void main()
{    
    vec4 sampled= vec4(1.0, 1.0, 1.0, texture(textArray, TexCoords).r);
    vec4 sampledTemp = texture(inputTexture, outPosition.xy);
    //Color implementation
    vec3 inputColor = texture(inputTexture, vec2(0.5)).rgb;
    color = sampledTemp * sampled;
    //not empty space
    //if (TexCoords.z > 0.1) {
    //    //black pixel
    //    if(sampled.w == 0.0 ) {
    //        color = bgColor;
    //    } else {
    //        color = vec4(charColor);
    //    }
    //}
    //else {
    //    //Make empty space blank
    //    color = vec4(charColor.xyz, 0.0);
    //}
}