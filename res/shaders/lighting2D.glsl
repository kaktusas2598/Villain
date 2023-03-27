#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoords;

out vec4 v_color;
out vec2 v_texCoords;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    v_color = color;
    v_texCoords = texCoords;
}

#shader fragment
#version 330 core

layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

void main() {
    // Calculate distance from center to pixel
    float distance = length(v_texCoords);
    //fragColor = vec4(v_color.rgb, v_color.a * (1.0 - distance));
    fragColor = vec4(v_color.rgb, v_color.a * (pow(0.01, distance) - 0.01));
}
