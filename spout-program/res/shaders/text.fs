#version 430 core
in vec2 glyphTexCoords;
layout(location = 0) out vec4 color;

layout(binding=0) uniform sampler2DArray textArray;
layout(binding=1) uniform sampler2D inputTexture;
layout(binding=2) uniform sampler2D sobelTexture;
layout(binding=3) uniform sampler2DArray edgeArray;

uniform vec4 charColor;
uniform vec4 bgColor;
uniform float charsetSize;
uniform ivec2 imgDims;
uniform int charSize;
uniform ivec2 outputSize;

#define M_PI 3.1415926535897932384626433832795

void main()
{   
    //Grab the RGB from the input image
    ivec2 inputTexturePos = ivec2(vec2(gl_FragCoord.x, imgDims.y * charSize - gl_FragCoord.y) / vec2(charSize));
    vec4 inputSample = texelFetch(inputTexture, inputTexturePos, 0);
    //Texture access here just determines which layer of glyph array we use to determine alpha.
    ivec2 pixelPosition = ivec2(mod(gl_FragCoord.x, charSize), mod(gl_FragCoord.y, charSize)) * textureSize(textArray, 0).xy / ivec2(charSize);
    
    vec4 sobelSample = texelFetch(sobelTexture, ivec2(gl_FragCoord.x / charSize, imgDims.y - gl_FragCoord.y / charSize), 0);
    if(sqrt(sobelSample.x*sobelSample.x + sobelSample.y*sobelSample.y) > 0.15) {
        float angle = sobelSample.z;
        int glyphIndex = 0;
        if(angle < 0.05 || angle < 0.7 && angle > 0.55) {
            glyphIndex = 2;
            //color = vec4(1., 0., 0., 1.);
        } else if(angle < 0.3 || angle < 0.8 && angle > 0.7) {
            glyphIndex = 1;
            //color = vec4(0., 1., 0., 1.);
        } else if(angle < 0.45 || angle < 0.95 && angle > 0.8) {
            glyphIndex = 3;
            //color = vec4(0., 0., 1., 1.);
        } else if(angle < 0.55 || angle < 0.05 && angle > 0.95) {
            glyphIndex = 0;
            //color = vec4(1., 1., 1., 1.);
        }
        vec4 glyphSample = vec4(1.0, 1.0, 1.0, texelFetch(edgeArray, ivec3(pixelPosition.x, textureSize(textArray, 0).y - pixelPosition.y, glyphIndex), 0).r);
        color = inputSample * glyphSample;
        if(glyphSample.w == 0.0) {
            color = bgColor;
        }
    } else {
        //Calculate luminance for use in sampling the glyph
        float luminance = float(inputSample.r * 0.2126 + inputSample.g * .7152 + inputSample.b * .0722) * (charsetSize);
        vec4 glyphSample = vec4(1.0, 1.0, 1.0, texelFetch(textArray, ivec3(pixelPosition.x, textureSize(textArray, 0).y - pixelPosition.y, luminance), 0).r);
        color = inputSample * glyphSample;
        if(luminance > 0.1 && glyphSample.w == 0.0) {
            color = bgColor;
        }
    }
}