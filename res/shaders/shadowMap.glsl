#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

// World transform matrix
uniform mat4 model;
// Light space matrix - light projection and view
uniform mat4 lightMatrix;

void main() {
    gl_Position = lightMatrix * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_color;

void main() {
    // frag colour shouldn't even be used for basic shadow map generation
    // this is for visualization of depth buffer
    o_color = vec4(gl_FragCoord.z);
}

