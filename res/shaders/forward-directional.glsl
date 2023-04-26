#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoords;
in vec4 v_shadowMapCoords;

in mat3 v_TBN;

#include lighting.glh
// Includes parralax mapping and shadow mapping calculations and shadowBias uniform
#include sampling.glh

uniform DirectionalLight dirLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

void main() {
    vec3 viewDirection = normalize(viewPosition - v_fragPos);
    vec2 texCoords = v_texCoords;
    vec3 normal = normalize(v_normal);

    // Parallax Mapping
    if (material.useDispMap) {
        texCoords = parallaxMapping(texCoords, viewDirection, material.texture_disp, material.dispMapScale, material.dispMapBias, v_TBN);
    }
    // Normal Mapping
    if (material.useNormalMap) {
        // transform normal vector to range [-1,1]
        // Or instead of 2.0f -> 255.0/128.0?
        normal = 2.0 * texture(material.texture_normal, texCoords).rgb - 1.0f;
        //normal = normalize(normal * 2.0 - 1.0);
        // More correct way using TBN matrix to convert tangent space normal to local space
        normal = normalize(v_TBN * normal);
    }

    vec3 outputColor = vec3(0.0);
    // Add light calculation multiplied by shadow map effect
    outputColor += calculateDirLight(dirLight, normal, v_fragPos, viewDirection, texCoords) * calcShadowAmount(shadowMap, v_shadowMapCoords);

    color = vec4(outputColor, 1.0);
}
