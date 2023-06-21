#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biTangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;
layout(location = 7) in mat4 instanceMatrix;

// Skeletal animation specific properties
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];
uniform bool skeletalAnimationEnabled = false;

// World transform matrix
uniform mat4 model;
// Light space matrix - light projection and view
uniform mat4 lightMatrix;

void main() {
    vec4 totalPosition = vec4(position, 1.0);
    if (skeletalAnimationEnabled) {
        totalPosition = vec4(0.0);
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++){
            // No bone set in this slot, so vertex is affected by less than 4 bones at least
            if (boneIds[i] == -1)
                continue;
            if (boneIds[i] >= MAX_BONES) {
                totalPosition = vec4(position, 1.0);
                break;
            }
            vec4 localPosition = finalBoneMatrices[boneIds[i]] * vec4(position, 1.0);
            totalPosition += localPosition * weights[i];
        }
    }

    gl_Position = lightMatrix * model * totalPosition;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_color;

void main() {
    // frag colour shouldn't even be used for basic shadow map generation
    // this is for visualization of depth buffer
    o_color = vec4(gl_FragCoord.z);
}

