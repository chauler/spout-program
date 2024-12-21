#version 430 core
in vec3 TexCoords;
in vec3 pixelColor;
in vec2 outPosition;
layout(location = 0) out vec4 color;

//uniform sampler2D text;
layout(binding=0) uniform sampler2DArray textArray;
layout(binding=1) uniform sampler2D inputTexture;
layout(binding=2) uniform sampler2D edgeTexture;
layout(binding=3) uniform sampler2DArray edgeArray;
uniform vec4 charColor;
uniform vec4 bgColor;

void main()
{   
    ivec2 downSampledPos = ivec2(floor(gl_FragCoord.x / 8), floor(gl_FragCoord.y / 8));
    //Colored text implementation
    vec4 sampled = vec4(0.0);
    vec4 sampledTemp = texture(inputTexture, outPosition.xy);
    vec4 edgeSample = texelFetch(edgeTexture, ivec2(downSampledPos.x, textureSize(edgeTexture, 0).y - downSampledPos.y), 0);//texture(edgeTexture, outPosition.xy);
    float edgeType = 0;
    if(edgeSample.z > 0.9) {
        edgeType = 0;
    }
    if(edgeSample.x < 0.1 && edgeSample.y > 0.9) {
        edgeType = 1;
    }
    if(edgeSample.x > 0.9 && edgeSample.y > 0.9) {
        edgeType = 2;
    }
    if(edgeSample.x > 0.9 && edgeSample.y < 0.1) {
        edgeType = 3;
    }

    //If pixel is an edge
    if(edgeSample.w > 0.9) {
        sampled = vec4(1.0, 1.0, 1.0, texture(edgeArray, vec3(TexCoords.xy, edgeType)).r);
        //sampled = vec4(1.0, 1.0, 1.0, texture(textArray, TexCoords).r);
        color = sampledTemp * sampled;
    } else {
        sampled = vec4(1.0, 1.0, 1.0, texture(textArray, TexCoords).r);
        color = sampledTemp * sampled;
    }
    //color = edgeSample * sampled;
    if(TexCoords.z > 0.1 && sampled.w == 0.0) {
        color = bgColor;
    }
}