#shader vertex
#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 MVP;

out float Height;
out vec2 TexCoords;

void main() {
    gl_Position = MVP * vec4(aPosition, 1.0);
    gl_Position.z = gl_Position.w;
    Height = aPosition.y;
    TexCoords = aTexCoords;
}

#shader fragment
#version 330 core

layout (location = 0) out vec4 FragColor;

uniform vec4 lowColor = vec4(0.6, 0.38, 0.66, 1.0);
uniform vec4 highColor = vec4(0.0, 0.15, 0.66, 1.0);
uniform sampler2D skydomeTexture;
uniform bool useTexture;

in float Height;
in vec2 TexCoords;

void main() {
    if (useTexture)
        FragColor = texture2D(skydomeTexture, TexCoords.xy);
    else
        FragColor = mix(lowColor, highColor, Height);
}
