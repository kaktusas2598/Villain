#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent; // Calculated by Assimp
layout(location = 4) in vec3 biTangent; // Calculated by Assimp

out vec2 v_texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_texCoords = texCoords;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_color;

in vec2 v_texCoords;

struct Material {
    vec4 diffuseColor;
    float shininess;

    bool useDiffuseMap;
    bool useSpecularMap;
    bool useNormalMap;
    bool useDispMap;

    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_disp;
};

uniform vec3 color; // Ambient light color
uniform Material material;

void main() {
    if (material.useDiffuseMap) {
        vec4 textureColor = texture2D(material.texture_diffuse, v_texCoords);
        o_color = textureColor * vec4(color, 1.0);
    } else {
        o_color = vec4(color, 1.0);
    }
}

