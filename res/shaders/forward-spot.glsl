#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biTangent;

out vec3 v_normal;
out vec3 v_fragPos;
out vec2 v_texCoords;

out mat3 v_TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    // Calculate fragment position for lighting in world space
    v_fragPos = vec3(model * vec4(position, 1.0));
    v_normal = normal;
    v_texCoords = texCoords;

    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(biTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    v_TBN = mat3(T, B, N);
    // Also if bitangent not passed to vertex shader, we can do this:
    // vec3 B = cross(N, T);
}

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
    vec3 norm;
    if (texture(material.texture_normal1, v_texCoords).rgb != vec3(0, 0, 0)) {
        norm = texture(material.texture_normal1, v_texCoords).rgb;
        // transform normal vector to range [-1,1]
        //norm = normalize(norm * 2.0 - 1.0);
        // More correct way using TBN matrix to convert tangent space normal to local space
        norm = norm * 2.0 - 1.0;
        norm = normalize(v_TBN * norm);
    } else {
        // No normal map
        norm = normalize(v_normal);
    }

    vec3 outputColor = vec3(0.0);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculateSpotLight(spotLight, norm, v_fragPos, viewDirection);

    color = vec4(outputColor, 1.0);
}
