#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 v_normal;
out vec3 v_fragPos;
out vec2 v_texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    // Calculate fragment position for lighting in world space
    v_fragPos = vec3(model * vec4(position, 1.0));
    v_normal = normal;
    v_texCoords = texCoords;
}

#shader fragment
#version 330 core
struct Material {
    vec4 diffuseColor;
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;

    // For models
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
};

struct BaseLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionalLight {
    vec3 direction;
    BaseLight base;
};

layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoords;

uniform Material material;
uniform DirectionalLight dirLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

BaseLight calculateLight(BaseLight base, vec3 normal, vec3 lightDirection, vec3 viewDirection);
vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDirection);

void main() {
    vec3 outputColor = vec3(0.0);
    vec3 norm = normalize(v_normal);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculateDirLight(dirLight, norm, viewDirection);

    color = vec4(outputColor, 1.0);
}

BaseLight calculateLight(BaseLight base, vec3 normal, vec3 lightDir, vec3 viewDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDirection = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess);

    BaseLight newLight;
    newLight.ambient = base.ambient * vec3(texture(material.texture_diffuse1, v_texCoords));
    newLight.diffuse = base.diffuse * diff * vec3(texture(material.texture_diffuse1, v_texCoords)) * material.diffuseColor.xyz;
    newLight.specular = base.specular * spec * vec3(texture(material.texture_specular1, v_texCoords));

    return newLight;
}

vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(-light.direction);

    BaseLight newLight = calculateLight(light.base, normal, lightDirection, viewDirection);

    return (newLight.ambient + newLight.diffuse + newLight.specular);
}
