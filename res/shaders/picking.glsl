#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

// MVP matrices
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out uvec3 outColor;

uniform uint objectIndex;
uniform uint drawIndex;

void main() {

    outColor = uvec3(objectIndex, drawIndex, gl_PrimitiveID);
}
