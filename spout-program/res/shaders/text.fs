#version 430 core
in vec2 TexCoords;
in vec2 outPosition;
layout(location = 0) out vec4 color;

layout(pixel_center_integer) in vec4 gl_FragCoord;

layout(binding=0) uniform sampler2DArray textArray;
layout(binding=1) uniform sampler2D inputTexture;
layout(binding=2) uniform usampler2D edgeTexture;
layout(binding=3) uniform sampler2DArray edgeArray;
uniform vec4 charColor;
uniform vec4 bgColor;
uniform int numChars = 16;
uniform int charSize = 8;

void main()
{   
    ivec2 downscaledTextureSize = textureSize(edgeTexture, 0).xy;
    ivec2 downSampledPos = ivec2(floor(gl_FragCoord.x / charSize), floor(gl_FragCoord.y / charSize));
    //Colored text implementation
    vec4 sampled = vec4(0.0);
    vec4 sampledTemp = texture(inputTexture, vec2(outPosition.x, 1 - outPosition.y));
    uvec4 edgeSample = texelFetch(edgeTexture, ivec2(downSampledPos.x, (downSampledPos.y)), 0);

    //If pixel is an edge
    if(edgeSample.x != 4) {
        sampled = vec4(1.0, 1.0, 1.0, texture(edgeArray, vec3(TexCoords, edgeSample.x)).r);
        color = sampledTemp * sampled;
    } else {
        sampled = vec4(1.0, 1.0, 1.0, texture(textArray, vec3(TexCoords, edgeSample.w / numChars)).r);
        color = sampledTemp * sampled;
    }
    //This is legacy, old feature that probably isn't useful
    if(edgeSample.w / numChars > 0.1 && sampled.w == 0.0) {
        color = bgColor;
    }
}