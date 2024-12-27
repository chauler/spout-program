#version 430 core
out vec4 FragColor;

uniform sampler2D inputTex;

const int samples = 35;
const int sLOD = 8;
const float sigma = float(samples) * .25;
// used in grayScale
const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

float gaussian(vec2 i) { return exp(-.5 * dot(i /= sigma, i)) / (6.28 * sigma * sigma); }

vec4 blur(vec2 U, vec2 scale) {
    vec4 O = vec4(0);
    int s = samples / sLOD;

    for (int i = 0; i < s * s; i++) {
        vec2 d = vec2(i % s, i / s) * float(sLOD) - float(samples) / 2.;
        vec4 temp = texture(inputTex, U + scale * d, 0);
        O += vec4(gaussian(d) * temp);
    }

    return O / O.a;
}

// XDoG parameters
uniform float Epsilon = 0.018; //White Threshold
uniform float Phi = 200.0; //How white the white is
uniform float Sigma = 0.15; //blurriness
uniform float k = 2.1; //Thickness of lines, higher = thicker
uniform float p = 0.98; //Increases = more/bigger edges

void main() {
    // Normalized pixel coordinates (from 0 to 1)
    ivec2 resolution = textureSize(inputTex, 0);
    vec2 uv = gl_FragCoord.xy / resolution;

    // Two Gaussian blur
    vec3 blurImage1 = blur(uv, vec2(k * Sigma / resolution)).rgb;
    vec3 blurImage2 = blur(uv, vec2(Sigma / resolution)).rgb;
    blurImage1 = vec3((blurImage1 * W).g);
    blurImage2 = vec3((blurImage2 * W).g);
    float diff = blurImage1.r  - p * blurImage2.r;

    if (diff < Epsilon) {
        diff = 0.0;
    } else {
        diff = 0.5 + tanh(Phi * diff);
    }

    // XDoG
    FragColor = vec4(vec3(13.0 * (diff)), 1.0);

    // Just DoG
    // FragColor = vec4(20.0*(blurImage1-blurImage2),1.0);
}