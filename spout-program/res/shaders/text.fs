#version 430 core
in vec3 TexCoords;
in vec3 pixelColor;
in vec2 outPosition;
layout(location = 0) out vec4 color;

//uniform sampler2D text;
layout(binding=0) uniform sampler2DArray textArray;
layout(binding=1) uniform sampler2D inputTexture;
layout(binding=2) uniform usampler2D edgeTexture;
layout(binding=3) uniform sampler2DArray edgeArray;
uniform vec4 charColor;
uniform vec4 bgColor;
uniform int numChars;

void main()
{   
    ivec2 downSampledPos = ivec2(floor(gl_FragCoord.x / 8), floor(gl_FragCoord.y / 8));
    //Colored text implementation
    vec4 sampled = vec4(0.0);
    vec4 sampledTemp = texture(inputTexture, outPosition.xy);
    uvec4 edgeSample = texelFetch(edgeTexture, ivec2(downSampledPos.x, textureSize(edgeTexture, 0).y - downSampledPos.y), 0);

    //If pixel is an edge
    if(edgeSample.x != 4) {
        sampled = vec4(1.0, 1.0, 1.0, texture(edgeArray, vec3(TexCoords.xy, edgeSample.x)).r);
        color = sampledTemp * sampled;
    } else {
        sampled = vec4(1.0, 1.0, 1.0, texture(textArray, vec3(TexCoords.xy, edgeSample.w / 16)).r);
        color = sampledTemp * sampled;
    }
    //color = edgeSample * sampled;
    if(TexCoords.z > 0.1 && sampled.w == 0.0) {
        color = bgColor;
    }
}