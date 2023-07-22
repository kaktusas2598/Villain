#shader vertex
#version 330 core

layout (location = 0) in vec3 position;

out vec3 v_texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 rotation;

void main() {
    v_texCoords = position;
    vec4 pos = projection * view * rotation * vec4(position, 1.0);
    gl_Position = pos.xyww;
}

#shader fragment
#version 330 core

out vec4 fragColor;

in vec3 v_texCoords; // Diretion vector representing 3D texture coordinate
uniform samplerCube cubemap;

void main() {
    fragColor = texture(cubemap, v_texCoords);
}
