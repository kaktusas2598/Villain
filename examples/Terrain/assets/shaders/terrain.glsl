#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

// For now no need for world/model matrix as terrain will be constructed using world coordinates and won't be rotated/translated
uniform mat4 projection;
uniform mat4 view;

uniform float minHeight;
uniform float maxHeight;

out vec4 color;
out vec2 outUV;

void main() {
    gl_Position = projection * view * vec4(position, 1.0);

    float deltaHeight = maxHeight - minHeight;
    float heightRatio = (position.y - minHeight) / deltaHeight;

    // Map color component from 0.2 to 1.0 so that the lowest parts are not completely black
    float c = heightRatio * 0.8 + 0.2;

    color = vec4(c, c, c, 1.0);
    outUV = uv;
}

#shader fragment
#version 330 core

in vec4 color;
in vec2 outUV;

uniform sampler2D terrainTexture;
uniform bool useTexture = false;

layout(location = 0) out vec4 outColor;

void main() {
    if (useTexture) {
        vec4 texColor = texture(terrainTexture, outUV);
        outColor = texColor * color;
    } else {
        outColor = color;
    }
}
