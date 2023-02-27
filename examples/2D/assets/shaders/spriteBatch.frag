#version 330 core

layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

uniform sampler2D spriteTexture;

void main() {
    fragColor = vec4(vec3(texture(spriteTexture, v_texCoords)), 1.0) * v_color;
}
