#shader vertex
#version 330 core

#include forwardLighting.vsh

#shader fragment
#version 330 core
layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

in vec2 v_texCoords;

uniform vec3 lightColor;

void main()
{
    frag_color = vec4(lightColor, 1.0);
    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bright_color = vec4(frag_color.rgb, 1.0);
	else
		bright_color = vec4(0.0, 0.0, 0.0, 1.0);
}
