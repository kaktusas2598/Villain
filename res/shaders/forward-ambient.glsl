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

    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_normal1;
    sampler2D texture_normal2;
};


// TODO: need to add getter/setter for this in RenderingEngine and add it to imgui
uniform vec3 color; // Ambient light color
uniform Material material;

void main() {
    vec4 textureColor = texture2D(material.texture_diffuse1, v_texCoords);
    if (textureColor == vec4(0, 0, 0, 0))
        o_color = vec4(color, 1.0);
    else
        o_color = textureColor * vec4(color, 1.0);
    o_color = texture2D(material.texture_diffuse1, v_texCoords);
}

