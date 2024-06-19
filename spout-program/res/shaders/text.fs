#version 330 core
in vec3 TexCoords;
layout(location = 0) out vec4 color;

//uniform sampler2D text;
uniform sampler2DArray textArray;
uniform vec3 textColor;

void main()
{    
    //vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    vec4 sampled= vec4(1.0, 1.0, 1.0, texture(textArray, TexCoords).r);
    //not empty space
    if (TexCoords.z > 0.1) {
        //black pixel
        if(sampled.w == 0.0 ) {
            color = vec4(128.0, 128.0, 128.0, 0.5);
        } else {
            //color = vec4(textColor, 1.0) * sampled;
            color = vec4(textColor, 1.0);
        }
    }
    else {
        color = vec4(textColor, 0.0);
    }
}