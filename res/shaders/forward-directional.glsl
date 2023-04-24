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
#include sampling.glh

uniform DirectionalLight dirLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

uniform float shadowBias;

float calcShadowAmount(sampler2D shadowMap, vec4 shadowMapCoords0) {
    // Perspective divide and transform to [0,1] range
    vec3 shadowMapCoords = (shadowMapCoords0.xyz / shadowMapCoords0.w);
    shadowMapCoords = shadowMapCoords * 0.5 + 0.5; // Or instead of this can construct and use shadowBiasMatrix
    // Make sure pixels out of view of light projection are clamped to 1, tested and works
    if (shadowMapCoords.z > 1.0) {
        shadowMapCoords.z = 1.0;
    }
    //float depth = texture(shadowMap, shadowMapCoords.xy).r;
    float bias = shadowBias;
    //float bias = max(shadowBias * (1.0 - dot(normalize(v_normal), normalize(-dirLight.direction))), 0.005);
    //float bias = max(shadowBias * (1.0 - dot(normalize(v_normal), normalize(dirLight.position - v_fragPos))), 0.005);

    // Without PCF
    //return (depth + bias) < shadowMapCoords.z ? 0.0 : 1.0;
    // WITHOUT bias
    //return depth < shadowMapCoords.z ? 0.0 : 1.0;
    //return sampleShadowMap(shadowMap, shadowMapCoords.xy, shadowMapCoords.z);

    // Apply PCF (Percentage-Closer Filter)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x){
        for (int y = -1; y <= 1; ++y){
            float depth = texture(shadowMap, shadowMapCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (depth + bias) < shadowMapCoords.z ? 0.0 : 1.0;
        }
    }
    return shadow / 9.0;
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
    // Add light calculation multiplied by shadow map effect
    outputColor += calculateDirLight(dirLight, normal, v_fragPos, viewDirection, texCoords) * calcShadowAmount(shadowMap, v_shadowMapCoords);

    color = vec4(outputColor, 1.0);
}
