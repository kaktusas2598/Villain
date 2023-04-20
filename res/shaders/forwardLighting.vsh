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

