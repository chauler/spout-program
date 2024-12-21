#version 430 core
in vec3 TexCoords;
in vec3 pixelColor;
in vec2 outPosition;
layout(location = 0) out vec4 color;

//uniform sampler2D text;
layout(binding=0) uniform sampler2DArray textArray;
layout(binding=1) uniform sampler2D inputTexture;
layout(binding=2) uniform sampler2D edgeTexture;
uniform vec4 charColor;
uniform vec4 bgColor;

void main()
{   
    //Colored text implementation
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textArray, TexCoords).r);
    vec4 sampledTemp = texture(inputTexture, outPosition.xy);
    color = sampledTemp * sampled;
    if(TexCoords.z > 0.1 && sampled.w == 0.0) {
        color = bgColor;
    }
}