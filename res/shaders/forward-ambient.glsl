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

uniform vec3 color; // Ambient light color
uniform sampler2D texture;

void main() {
    vec4 textureColor = texture2D(texture, v_texCoords);
    if (textureColor == vec4(0, 0, 0, 0))
        o_color = vec4(color, 1.0);
    else
        o_color = textureColor * vec4(color, 1.0);
}

