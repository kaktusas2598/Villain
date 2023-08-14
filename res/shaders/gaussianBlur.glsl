#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core

layout(location = 0) out vec4 frag_color;

in vec2 v_texCoords;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texOffset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, v_texCoords).rgb * weight[0]; // current fragment's contribution

    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
            result += texture(image, v_texCoords + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, v_texCoords - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < 5; ++i) {
            result += texture(image, v_texCoords + vec2(0.0, texOffset.y * i)).rgb * weight[i];
            result += texture(image, v_texCoords - vec2(0.0, texOffset.y * i)).rgb * weight[i];
        }
    }

    frag_color = vec4(result, 1.0);
}
