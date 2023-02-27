#version 330 core

layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

uniform sampler2D spriteTexture;

void main() {
    fragColor = texture(spriteTexture, v_texCoords) * v_color;
}
