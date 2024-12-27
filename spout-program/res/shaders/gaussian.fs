#version 430 core
out vec4 FragColor;

uniform sampler2D inputTex;

const int samples = 35;
const int sLOD = 1;
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
const float Epsilon = 0.01;
const float Phi = 200.0;
const float Sigma = 0.2;
const float k = 1.5;
const float p = 0.97;

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