#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoords;
uniform sampler2D texture1;

uniform bool invertColors;
uniform bool grayScale;
uniform bool sharpen;
uniform bool blur;
uniform bool edgeDetection; // or outline

// For kernel effects, feel free to tweak
const float offset = 1.0 / 300.0;

// FIXME: make sure you can apply multiple post fx at once
void main() {
    color = texture(texture1, v_texCoords);

    if (invertColors) {
        color = vec4(vec3(1.0 - texture(texture1, v_texCoords)), 1.0);
    }

    if (grayScale) {
        // To create grayscale we simply average all colors
        //float average = (color.r, color.g, color.b) / 3.0;
        // Or to create results more precise to the human vision, we use weighted color channels, as human eyes
        // sees more blue than red
        float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        color = vec4(average, average, average, 1.0);
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
        sampleTex[i] = vec3(texture(texture1, v_texCoords.st + offsets[i]));
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
    if (sharpen) {
        for (int i = 0; i < 9; i++) {
            col += sampleTex[i] * sharpenKernel[i];
        }

        color = vec4(col, 1.0);
    }

    if (blur) {
        for (int i = 0; i < 9; i++) {
            col += sampleTex[i] * blurKernel[i];
        }

        color = vec4(col, 1.0);
    }

    if (edgeDetection) {
        for (int i = 0; i < 9; i++) {
            col += sampleTex[i] * edgeDetectionKernel[i];
        }

        color = vec4(col, 1.0);
    }
}
