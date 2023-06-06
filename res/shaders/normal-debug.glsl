#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biTangent;
layout(location = 5) in mat4 instanceMatrix;

out DATA {
    vec3 normal; // If using normal maps, not needed
    mat4 projection;
} dataOut;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool instancedRenderingEnabled = false;

void main() {
    mat4 worldTransform = model;
    if (instancedRenderingEnabled) {
        worldTransform = model * instanceMatrix;
    }

    vec3 fragPos = vec3(worldTransform * vec4(position, 1.0));
    // Projection will be applied in geometry shader
    gl_Position = view * vec4(fragPos, 1.0);
    // Does normal needs to be translated to world space here?
    dataOut.normal = normal;
    dataOut.projection = projection;
}

#shader geometry
#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in DATA {
    vec3 normal; // If using normal maps, not needed
    mat4 projection;
} dataIn[];

void main() {

    gl_Position = dataIn[0].projection * gl_in[0].gl_Position;
    EmitVertex();
    // NOTE: originally offset was 0.01 instead of 0.1 for all vertices but normals were too small to see
    gl_Position = dataIn[0].projection * (gl_in[0].gl_Position + 0.1f * vec4(dataIn[0].normal, 0.0f));
    EmitVertex();
    EndPrimitive();

    gl_Position = dataIn[1].projection * gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = dataIn[1].projection * (gl_in[1].gl_Position + 0.1f * vec4(dataIn[1].normal, 0.0f));
    EmitVertex();
    EndPrimitive();

    gl_Position = dataIn[2].projection * gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = dataIn[2].projection * (gl_in[2].gl_Position + 0.1f * vec4(dataIn[2].normal, 0.0f));
    EmitVertex();
    EndPrimitive();
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_color;

void main() {
	o_color = vec4(0.8f, 0.3f, 0.02f, 1.0f);
}

