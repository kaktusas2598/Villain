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
    //gl_Position = projection * view * model * vec4(position, 1.0);
    gl_Position = view * vec4(position, 1.0);
    v_color = color;
    v_texCoords = texCoords;
}
