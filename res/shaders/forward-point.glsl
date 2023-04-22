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

uniform PointLight pointLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

void main() {
    vec3 normal;
    if (material.useNormalMap) {
        // transform normal vector to range [-1,1]
        // Or instead of 2.0f -> 255.0/128.0?
        normal = 2.0 * texture(material.texture_normal, v_texCoords).rgb - 1.0f;
        //normal = normalize(normal * 2.0 - 1.0);
        // More correct way using TBN matrix to convert tangent space normal to local space
        normal = normalize(v_TBN * normal);
    } else {
        normal = normalize(v_normal);
    }

    vec3 outputColor = vec3(0.0);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculatePointLight(pointLight, normal, v_fragPos, viewDirection);

    color = vec4(outputColor, 1.0);
}
