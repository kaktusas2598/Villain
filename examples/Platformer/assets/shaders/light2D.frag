#version 330 core

layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

void main() {
    // Calculate distance from center to pixel
    float distance = length(v_texCoords);
    //fragColor = vec4(v_color.rgb, v_color.a * (1.0 - distance));
    fragColor = vec4(v_color.rgb, v_color.a * (pow(0.01, distance) - 0.01));
}
