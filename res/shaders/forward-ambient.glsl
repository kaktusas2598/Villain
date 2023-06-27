#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_color;

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

void main() {
    // NOTE: Need to find nicer way to here to avoid all these if statements
    // Skeletal animation debug
    bool found = false;

    // Color pixels depending on weights, bone is selected using displayBoneIndex
    if (boneWeightDebugEnabled) {
        for(int i = 0; i < 4; i++){
            if (v_boneIds[i] == displayBoneIndex) {
                if (v_weights[i] >= 0.7) {
                    o_color = vec4(1.0, 0.0, 0.0, 1.0) * v_weights[i];
                } else if (v_weights[i] >= 0.4 && v_weights[i] <= 0.7) {
                    o_color = vec4(0.0, 1.0, 0.0, 1.0) * v_weights[i];
                } else if (v_weights[i] >= 0.1) {
                    o_color = vec4(1.0, 1.0, 0.0, 1.0) * v_weights[i];
                }

                found = true;
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

        if (material.useDiffuseMap) {
            vec4 textureColor = texture2D(material.texture_diffuse, texCoords);
            o_color = textureColor * vec4(ambientLight, 1.0);
        } else {
            o_color = vec4(ambientLight, 1.0) * material.diffuseColor;
        }

        if (fogColor != vec3(0.0)) {
            o_color = mix(vec4(fogColor, 1.0), o_color, visibility);
        }

        if (selected) {
            o_color = o_color * vec4(0.0, 1.0, 0.0, 1.0);
        }
    }
}

