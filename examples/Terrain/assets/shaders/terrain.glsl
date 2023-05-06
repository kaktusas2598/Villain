#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

// For now no need for world/model matrix as terrain will be constructed using world coordinates and won't be rotated/translated
uniform mat4 projection;
uniform mat4 view;

out vec4 color;

void main() {
    gl_Position = projection * view * vec4(position, 1.0);

    // HACK: using current height map top height to generate gradient of colours
    color = vec4(position.y / 200.0);
}

#shader fragment
#version 330 core

in vec4 color;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = color;
}
