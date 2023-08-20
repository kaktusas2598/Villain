#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 bright_color;

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoords;
in float visibility;

flat in ivec4 v_boneIds;
in vec4 v_weights;

in mat3 v_TBN;

#include lighting.glh
#include sampling.glh

uniform vec3 ambientLight;
uniform vec3 fogColor;
uniform vec3 viewPosition;

uniform bool selected = false;
uniform bool boneWeightDebugEnabled;
uniform int displayBoneIndex;

uniform bool normalMapDebugEnabled;

uniform bool uvDebugEnabled;
uniform sampler2D uvCheckerboard;

void main() {
    // NOTE: Need to find nicer way to here to avoid all these if statements
    // Skeletal animation debug
    bool found = false;

    // Color pixels depending on weights, bone is selected using displayBoneIndex
    if (boneWeightDebugEnabled) {
        for(int i = 0; i < 4; i++){
            if (v_boneIds[i] == displayBoneIndex) {
                if (v_weights[i] >= 0.7) {
                    frag_color = vec4(1.0, 0.0, 0.0, 1.0) * v_weights[i];
                } else if (v_weights[i] >= 0.4 && v_weights[i] <= 0.7) {
                    frag_color = vec4(0.0, 1.0, 0.0, 1.0) * v_weights[i];
                } else if (v_weights[i] >= 0.1) {
                    frag_color = vec4(1.0, 1.0, 0.0, 1.0) * v_weights[i];
                }

                found = true;
                bright_color = vec4(0.0, 0.0, 0.0, 1.0);
                break;
            }
        }
    }

    if (!found) {
        vec3 viewDirection = normalize(viewPosition - v_fragPos);
        vec2 texCoords = v_texCoords;
        // Parallax Mapping
        if (material.useDispMap) {
            texCoords = parallaxMapping(texCoords, viewDirection, material.texture_disp, material.dispMapScale, material.dispMapBias, v_TBN);
        }

        if (usePBR) {
            if (pbrMaterial.useAlbedoMap) {
                vec4 textureColor = texture2D(pbrMaterial.texture_albedo, texCoords);
                frag_color = textureColor * vec4(ambientLight, 1.0);
            } else {
                frag_color = vec4(ambientLight * pbrMaterial.albedo, 1.0);
            }

        } else {
            if (material.useDiffuseMap) {
                vec4 textureColor = texture2D(material.texture_diffuse, texCoords);
                frag_color = textureColor * vec4(ambientLight, 1.0);
            } else {
                frag_color = vec4(ambientLight, 1.0) * material.diffuseColor;
            }
        }

        if (fogColor != vec3(0.0)) {
            frag_color = mix(vec4(fogColor, 1.0), frag_color, visibility);
        }

        if (selected) {
            frag_color = frag_color * vec4(0.0, 1.0, 0.0, 1.0);
        }

        if (normalMapDebugEnabled) {
            vec3 normal = normalize(v_normal);
            if (material.useNormalMap || pbrMaterial.useNormalMap) {
                // transform normal vector to range [-1,1]
                // Or instead of 2.0f -> 255.0/128.0?
                if (usePBR)
                    normal = 2.0 * texture(pbrMaterial.texture_normal, texCoords).rgb - 1.0f;
                else
                    normal = 2.0 * texture(material.texture_normal, texCoords).rgb - 1.0f;
                // More correct way using TBN matrix to convert tangent space normal to local space
                normal = normalize(v_TBN * normal);
            }
            frag_color = vec4(normal, 1.0);
        }

        if (uvDebugEnabled) {
            frag_color = texture2D(uvCheckerboard, texCoords);
        }

        // Check whether fragment ouput is higher then specified threshold and output brightness colour used for bloom
        float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
        if (brightness > 1.0)
            bright_color = vec4(frag_color.rgb, 1.0);
        else
            bright_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
