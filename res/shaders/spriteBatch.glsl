#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoords;

out vec4 v_color;
out vec2 v_texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_color = color;
    v_texCoords = texCoords;
}

#shader fragment
#version 330 core

layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

uniform sampler2D spriteTexture;

void main() {
    //fragColor = texture(spriteTexture, v_texCoords) * v_color;
    vec4 color = texture(spriteTexture, v_texCoords) * v_color;
    //HACK: Can not get transparency working for layering multiple sprites, but this will be
    //bad once the lighting comes in?
    if (color.a < 0.5) discard; else fragColor = color;
}
