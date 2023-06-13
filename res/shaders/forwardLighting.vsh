layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biTangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;
layout(location = 7) in mat4 instanceMatrix;

out vec3 v_normal; // If using normal maps, not needed
out vec3 v_fragPos; // World position
out vec2 v_texCoords;
out vec4 v_shadowMapCoords; // Used for directional shadow mapping for dir and spot lights
out mat3 v_TBN; // Tangent-Bitangent-Normal matrix

// Skeletal animation debug
flat out ivec4 v_boneIds;
out vec4 v_weights;

// MVP matrices
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// Skeletal animation specific properties
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];
uniform bool skeletalAnimationEnabled = false;

uniform bool instancedRenderingEnabled = false;

out float visibility; // Determines how foggy vertex is
#include fog.glh

// TODO: shadow map stuff is not needed in ambient shader
uniform mat4 lightMatrix;

void main() {
    mat4 worldTransform = model;

    if (instancedRenderingEnabled) {
        worldTransform = model * instanceMatrix;
    }

    vec4 totalPosition = vec4(position, 1.0);
    if (skeletalAnimationEnabled) {
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++){
            if (boneIds[i] == -1)
                continue;
            if (boneIds[i] >= MAX_BONES) {
                totalPosition = vec4(position, 1.0);
                break;
            }
            vec4 localPosition = finalBoneMatrices[boneIds[i]] * vec4(position, 1.0);
            totalPosition += localPosition * weights[i];
            // TODO: How to set normals properly for animated models
            vec3 localNormal = mat3(finalBoneMatrices[boneIds[i]]) * normal;
        }
    }

    v_boneIds = boneIds;
    v_weights = weights;

    // Calculate fragment position for lighting in world space
    v_fragPos = vec3(worldTransform * totalPosition);
    gl_Position = projection * view * vec4(v_fragPos, 1.0);
    // Does normal needs to be translated to world space here?
    v_normal = normal;
    v_texCoords = texCoords;
    v_shadowMapCoords = lightMatrix * vec4(v_fragPos, 1.0);

    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(vec3(worldTransform * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(worldTransform * vec4(normal, 0.0)));

    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(T, N); // or N, T ??
    /*Or cheaper way : vec3 B = normalize(vec3(model * vec4(biTangent, 0.0)));*/

    v_TBN = mat3(T, B, N);

    // FOG
    if (fogColor != vec3(0.0)) {
        vec4 relativeToCamera = view * vec4(v_fragPos, 1.0);
        float distance = length(relativeToCamera.xyz);
        visibility = exp(-pow(distance * fogDensity, fogGradient));
        visibility = clamp(visibility, 0.1, 1.0);
    }
}
