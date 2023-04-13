#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoords;

#include lighting.glh

uniform DirectionalLight dirLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

void main() {
    vec3 outputColor = vec3(0.0);
    vec3 norm = normalize(v_normal);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculateDirLight(dirLight, norm, viewDirection);

    color = vec4(outputColor, 1.0);
}
