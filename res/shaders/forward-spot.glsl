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

uniform SpotLight spotLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

void main() {
    vec3 normal;
    if (texture(material.texture_normal1, v_texCoords).rgb != vec3(0, 0, 0)) {
        normal = texture(material.texture_normal1, v_texCoords).rgb;
        // transform normal vector to range [-1,1]
        //normal = normalize(normal * 2.0 - 1.0);
        // More correct way using TBN matrix to convert tangent space normal to local space
        normal = normal * 2.0 - 1.0;
        normal = normalize(v_TBN * normal);
    } else {
        // No normal map
        normal = normalize(v_normal);
    }

    vec3 outputColor = vec3(0.0);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculateSpotLight(spotLight, normal, v_fragPos, viewDirection);

    color = vec4(outputColor, 1.0);
}
