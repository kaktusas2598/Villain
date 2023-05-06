#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

// For now no need for world/model matrix as terrain will be constructed using world coordinates and won't be rotated/translated
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(position, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0);
}
