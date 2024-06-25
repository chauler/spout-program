#version 330 core
in vec3 TexCoords;
in vec3 pixelColor;
layout(location = 0) out vec4 color;

//uniform sampler2D text;
uniform sampler2DArray textArray;
uniform vec4 charColor;
uniform vec4 bgColor;

void main()
{    
    vec4 sampled= vec4(1.0, 1.0, 1.0, texture(textArray, TexCoords).r);

    //Color implementation
    color = vec4(pixelColor , 1.0) * sampled;

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