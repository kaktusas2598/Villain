#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;

// Post Processing Effects
uniform bool invertColors;
uniform bool grayScale;
uniform bool sharpen;
uniform bool blur;
uniform bool edgeDetection; // or outline

// Gamma Correction
uniform bool gammaCorrection;

// HDR
uniform bool hdr;
uniform float exposure;

// For kernel effects, feel free to tweak
const float offset = 1.0 / 300.0;

void main() {
    color = texture(scene, v_texCoords);

    vec3 hdrColor = texture(scene, v_texCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, v_texCoords).rgb;
    if(bloom)
        hdrColor += bloomColor; // additive blending

    if (invertColors) {
        hdrColor = 1.0 - hdrColor;
    }

    if (grayScale) {
        // To create grayscale we simply average all colors
        //float average = (color.r, color.g, color.b) / 3.0;
        // Or to create results more precise to the human vision, we use weighted color channels, as human eyes
        // sees more blue than red
        float average = dot(hdrColor.rgb, vec3(0.2126, 0.7152, 0.0722));
        hdrColor = vec3(average);
    }

    vec2 offsets[9] = vec2[] (
        vec2(-offset, offset), // top-left
        vec2(   0.0f, offset), // top-center
        vec2( offset, offset), // top-right
        vec2(-offset, 0.0f), // center-left
        vec2(   0.0f, 0.0f), // center-center
        vec2( offset, 0.0f), // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(   0.0f, -offset), // bottom-center
        vec2( offset, -offset) // bottom-right
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(scene, v_texCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);

    // Kernels (convolution matrix)
    float sharpenKernel[9] = float[] (
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    float blurKernel[9] = float[] (
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    float edgeDetectionKernel[9] = float[] (
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    // Kernel effects
    // NOTE: Do not mix with other effects, HDR or gamma correction in here
    if (sharpen) {
        for (int i = 0; i < 9; i++) {
            col += sampleTex[i] * sharpenKernel[i];
        }
        hdrColor = col;
    }

    if (blur) {
        for (int i = 0; i < 9; i++) {
            col += sampleTex[i] * blurKernel[i];
        }
        hdrColor = col;
    }

    if (edgeDetection) {
        for (int i = 0; i < 9; i++) {
            col += sampleTex[i] * edgeDetectionKernel[i];
        }
        hdrColor = col;
    }

    const float gamma = 2.2;
    if (hdr) {
        // exposure tone mapping
        vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
        // gamma correction
        mapped = pow(mapped, vec3(1.0 / gamma));
        color = vec4(mapped, 1.0);
    }

    // apply gamma correction, must be done as the last step
    if (gammaCorrection && !hdr) {
        color.rgb = pow(hdrColor.rgb, vec3(1.0/gamma));
    }
}
