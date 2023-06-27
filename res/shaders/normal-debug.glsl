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

out DATA {
    vec3 normal; // If using normal maps, not needed
    mat4 projection;
} dataOut;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// Skeletal animation specific properties
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];
uniform bool skeletalAnimationEnabled = false;

uniform bool instancedRenderingEnabled = false;

void main() {
    mat4 worldTransform = model;
    if (instancedRenderingEnabled) {
        worldTransform = model * instanceMatrix;
    }

    vec4 totalPosition = vec4(position, 1.0);
    vec3 totalNormal = normal;
    if (skeletalAnimationEnabled) {
        totalPosition = vec4(0.0);
        totalNormal = vec3(0.0);
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
            vec3 localNormal = mat3(finalBoneMatrices[boneIds[i]]) * normal;
            totalNormal += localNormal * weights[i];
        }
    }

    vec3 fragPos = vec3(worldTransform * totalPosition);
    // Projection will be applied in geometry shader
    gl_Position = view * vec4(fragPos, 1.0);
    // Does normal needs to be translated to world space here?
    dataOut.normal = totalNormal;
    dataOut.projection = projection;
}

#shader geometry
#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in DATA {
    vec3 normal; // If using normal maps, not needed
    mat4 projection;
} dataIn[];

void main() {

    gl_Position = dataIn[0].projection * gl_in[0].gl_Position;
    EmitVertex();
    // NOTE: originally offset was 0.01 instead of 0.1 for all vertices but normals were too small to see
    gl_Position = dataIn[0].projection * (gl_in[0].gl_Position + 0.1f * vec4(dataIn[0].normal, 0.0f));
    EmitVertex();
    EndPrimitive();

    gl_Position = dataIn[1].projection * gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = dataIn[1].projection * (gl_in[1].gl_Position + 0.1f * vec4(dataIn[1].normal, 0.0f));
    EmitVertex();
    EndPrimitive();

    gl_Position = dataIn[2].projection * gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = dataIn[2].projection * (gl_in[2].gl_Position + 0.1f * vec4(dataIn[2].normal, 0.0f));
    EmitVertex();
    EndPrimitive();
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_color;

void main() {
	o_color = vec4(0.8f, 0.3f, 0.02f, 1.0f);
}

