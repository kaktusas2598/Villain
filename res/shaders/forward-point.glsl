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
#include sampling.glh

uniform PointLight pointLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

uniform samplerCube shadowCubeMap;
uniform float farPlane;

float calcShadowAmount(PointLight light, samplerCube shadowMap, vec3 fragPos) {
    vec3 fragToLight = fragPos - light.position;
    float closestDepth = texture(shadowMap, fragToLight).r;
    closestDepth *= farPlane;
    float currentDepth = length(fragToLight);

    float bias = shadowBias;
    // NO PCF
    //return (currentDepth - bias) > closestDepth ? 0.0 : 1.0;

    float shadow  = 0.0;
    float samples = 3.0; // NOTE: with 4 samples, FPS drops, but so far 3 samples produces nice results
    float offset  = 0.1;
    // PCF Shadows
    for(float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                float closestDepth = texture(shadowMap, fragToLight + vec3(x, y, z)).r;
                closestDepth *= farPlane;   // undo mapping [0;1]
                if(currentDepth - bias < closestDepth)
                    shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
}

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
    outputColor += calculatePointLight(pointLight, normal, v_fragPos, viewDirection, texCoords) * calcShadowAmount(pointLight, shadowCubeMap, v_fragPos);

    color = vec4(outputColor, 1.0);
}
