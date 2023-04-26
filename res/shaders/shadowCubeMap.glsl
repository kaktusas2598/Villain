#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

// World transform matrix
uniform mat4 model;

void main() {
    gl_Position = model * vec4(position, 1.0);
}

#shader geometry
#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 fragPos;

void main() {
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face; // built-int var specifies face we are rendering to
        for (int i = 0; i < 3; ++i) {
            fragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * fragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#shader fragment
#version 330 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    // distance between fragment and light source
    float lightDistance = length(fragPos.xyz - lightPos);

    // map to [0,1] range
    lightDistance = lightDistance / farPlane;

    gl_FragDepth = lightDistance;
}

