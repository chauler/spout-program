#version 430 core
in vec2 glyphTexCoords;
layout(location = 0) out vec4 color;

layout(binding=0) uniform sampler2DArray textArray;
layout(binding=1) uniform sampler2D inputTexture;
uniform vec4 charColor;
uniform vec4 bgColor;
uniform float charsetSize;
uniform ivec2 imgDims;
uniform int charSize;

void main()
{   
    //Grab the RGB from the input image
    vec4 inputSample = texelFetch(inputTexture, ivec2(vec2(gl_FragCoord.x, imgDims.y * charSize - gl_FragCoord.y) / vec2(charSize)), 0);
    //Calculate luminance for use in sampling the glyph
    float luminance = float(inputSample.r * 0.2126 + inputSample.g * .7152 + inputSample.b * .0722) * (charsetSize - 1);
    //Texture access here just determines which layer of glyph array we use to determine alpha.
    ivec2 pixelPosition = ivec2(mod(gl_FragCoord.x, charSize), mod(gl_FragCoord.y, charSize)) * textureSize(textArray, 0).xy / ivec2(charSize);
    vec4 glyphSample= vec4(1.0, 1.0, 1.0, texelFetch(textArray, ivec3(pixelPosition, luminance), 0).r);
    color = inputSample * glyphSample;
    if(luminance > 0.1 && glyphSample.w == 0.0) {
        color = bgColor;
    }
}