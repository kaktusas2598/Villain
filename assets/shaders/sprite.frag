#version 330 core

layout (location = 0) out vec4 fragColor;

in vec2 v_texCoords;

uniform sampler2D spriteTexture;

void main() {
    // TODO: need to pass in color as well either as vertex attrib or uniform?
    fragColor = vec4(vec3(texture(spriteTexture, v_texCoords)), 1.0) * vec4(0.0, 1.0, 0.0, 1.0);
}
