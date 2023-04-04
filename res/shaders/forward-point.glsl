#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 v_normal;
out vec3 v_fragPos;
out vec2 v_texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    // Calculate fragment position for lighting in world space
    v_fragPos = vec3(model * vec4(position, 1.0));
    v_normal = normal;
    v_texCoords = texCoords;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoords;

#include lighting.glh

uniform PointLight pointLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

void main() {
    vec3 outputColor = vec3(0.0);
    vec3 norm = normalize(v_normal);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculatePointLight(pointLight, norm, v_fragPos, viewDirection);

    color = vec4(outputColor, 1.0);
}
