#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 localPos;

void main()
{
    localPos = aPos;

    mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 localPos;

uniform samplerCube environmentMap;
uniform float exposure;

void main()
{
    vec3 envColor = texture(environmentMap, localPos).rgb;
    // Test pre-filtered map
    //vec3 envColor = textureLod(environmentMap, localPos, 0.2).rgb;
    //vec3 envColor = textureLod(environmentMap, localPos, 1.2).rgb;

    // Exposure mapping
    envColor = vec3(1.0) - exp(-envColor * exposure);
    // Reinhard tone mapping
    //envColor = envColor / (envColor + vec3(1.0));
    // Gamma correction
    envColor = pow(envColor, vec3(1.0/2.2));

    FragColor = vec4(envColor, 1.0);
}
