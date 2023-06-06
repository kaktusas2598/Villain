#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biTangent;
layout(location = 5) in mat4 instanceMatrix;

out DATA {
    vec3 normal; // If using normal maps, not needed
    vec2 uv;
    mat4 projection;

    vec3 fragPos; // World position
    vec4 shadowMapCoords; // Used for directional shadow mapping for dir and spot lights
    mat3 TBN; // Tangent-Bitangent-Normal matrix
    float visibility; // How foggy vertex is
} dataOut;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool instancedRenderingEnabled = false;

#include fog.glh

// TODO: shadow map stuff is not needed in ambient shader
uniform mat4 lightMatrix;

void main() {
    mat4 worldTransform = model;
    if (instancedRenderingEnabled) {
        worldTransform = model * instanceMatrix;
    }

    // Calculate fragment position for lighting in world space
    dataOut.fragPos = vec3(worldTransform * vec4(position, 1.0));
    // Projection will be applied in geometry shader
    gl_Position = view * vec4(dataOut.fragPos, 1.0);
    // Does normal needs to be translated to world space here?
    dataOut.normal = normal;
    dataOut.uv = texCoords;
    dataOut.projection = projection;
    dataOut.shadowMapCoords = lightMatrix * vec4(dataOut.fragPos, 1.0);

    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(vec3(worldTransform * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(worldTransform * vec4(normal, 0.0)));

    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(T, N); // or N, T ??
    /*Or cheaper way : vec3 B = normalize(vec3(model * vec4(biTangent, 0.0)));*/

    dataOut.TBN = mat3(T, B, N);

    // FOG
    if (fogColor != vec3(0.0)) {
        vec4 relativeToCamera = view * vec4(dataOut.fragPos, 1.0);
        float distance = length(relativeToCamera.xyz);
        dataOut.visibility = exp(-pow(distance * fogDensity, fogGradient));
        dataOut.visibility = clamp(dataOut.visibility, 0.1, 1.0);
    }
}

#shader geometry
#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

out vec3 normal;
out vec2 uv;
out vec3 fragPos;
out float visibility;
out mat3 TBN;

in DATA {
    vec3 normal; // If using normal maps, not needed
    vec2 uv;
    mat4 projection;

    vec3 fragPos; // World position
    vec4 shadowMapCoords; // Used for directional shadow mapping for dir and spot lights
    mat3 TBN; // Tangent-Bitangent-Normal matrix
    float visibility; // How foggy vertex is
} dataIn[];

void main() {

    gl_Position = dataIn[0].projection * gl_in[0].gl_Position;
    normal = dataIn[0].normal;
    uv = dataIn[0].uv;
    fragPos = dataIn[0].fragPos;
    visibility = dataIn[0].visibility;
    TBN = dataIn[0].TBN;
    EmitVertex();

    gl_Position = dataIn[1].projection * gl_in[1].gl_Position;
    normal = dataIn[1].normal;
    uv = dataIn[1].uv;
    fragPos = dataIn[1].fragPos;
    visibility = dataIn[1].visibility;
    TBN = dataIn[1].TBN;
    EmitVertex();

    gl_Position = dataIn[2].projection * gl_in[2].gl_Position;
    normal = dataIn[2].normal;
    uv = dataIn[2].uv;
    fragPos = dataIn[2].fragPos;
    visibility = dataIn[2].visibility;
    TBN = dataIn[2].TBN;
    EmitVertex();

    EndPrimitive();
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_color;

in vec3 normal;
in vec3 fragPos;
in vec2 uv;
in float visibility;
in mat3 TBN;

#include lighting.glh
#include sampling.glh

uniform vec3 ambientLight;
uniform vec3 fogColor;
uniform vec3 viewPosition;

uniform bool selected = false;

void main() {
    vec3 viewDirection = normalize(viewPosition - fragPos);
    vec2 texCoords = uv;
    // Parallax Mapping
    //if (material.useDispMap) {
        //texCoords = parallaxMapping(texCoords, viewDirection, material.texture_disp, material.dispMapScale, material.dispMapBias, v_TBN);
    //}

    if (material.useDiffuseMap) {
        vec4 textureColor = texture2D(material.texture_diffuse, texCoords);
        o_color = textureColor * vec4(ambientLight, 1.0);
    } else {
        o_color = vec4(ambientLight, 1.0);
    }

    if (fogColor != vec3(0.0)) {
        o_color = mix(vec4(fogColor, 1.0), o_color, visibility);
    }

    if (selected) {
        o_color = o_color * vec4(0.0, 1.0, 0.0, 1.0);
    }

}

