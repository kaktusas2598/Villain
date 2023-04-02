#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

out vec3 v_normal;
out vec3 v_fragPos;
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

    sampler2D texture_ambient1;
    sampler2D texture_ambient2;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_normal1;
    sampler2D texture_normal2;
};

uniform vec3 color; // Ambient light color
uniform Material material;

void main() {
    vec4 textureColor = texture2D(material.texture_diffuse1, v_texCoords);
    if (textureColor == vec4(0, 0, 0, 0))
        o_color = vec4(color, 1.0);
    else
        o_color = textureColor * vec4(color, 1.0);
}

