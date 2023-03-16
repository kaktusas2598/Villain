#version 330 core

layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

uniform sampler2D spriteTexture;

void main() {
    // Freetype glyphs are single color stored 8bit so only sampling r channel below
    vec4 color = texture(spriteTexture, v_texCoords).r * v_color;
    //HACK: Can not get transparency working for layering multiple sprites, but this will be
    //bad once the lighting comes in?
    if (color.a < 0.5) discard; else fragColor = color;
}
