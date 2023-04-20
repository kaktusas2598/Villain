#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoords;

in mat3 v_TBN;

#include lighting.glh

uniform DirectionalLight dirLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

void main() {
    vec3 norm;
    if (texture(material.texture_normal1, v_texCoords).rgb != vec3(0, 0, 0)) {
        norm = texture(material.texture_normal1, v_texCoords).rgb;
        // transform normal vector to range [-1,1]
        //norm = normalize(norm * 2.0 - 1.0);
        // More correct way using TBN matrix to convert tangent space normal to local space
        norm = norm * 2.0 - 1.0;
        norm = normalize(v_TBN * norm);
    } else {
        // No normal map
        norm = normalize(v_normal);
    }

    vec3 outputColor = vec3(0.0);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculateDirLight(dirLight, norm, viewDirection);

    color = vec4(outputColor, 1.0);
}
