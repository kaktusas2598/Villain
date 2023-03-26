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
    // Example of normal matrix calculation ( in world space, htat's why its derived from model matrix)
    // inverse is a very costly operation for shaders though!
    // But this must be used if we apply non uniform scale
    //v_normal = mat3(transpose(inverse(model))) * normal
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

struct PointLight {
    BaseLight base;
    vec3 position;

    // attenuation
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    BaseLight base;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
};


layout(location = 0) out vec4 color;

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoords;

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
// Alternatively we can calculate lighting in view space, this uniform becaumes 0,0,0 always and is not needed
// but we need to convert all relevant vectors, view matrix and normal matrix if used
uniform vec3 viewPosition;

BaseLight calculateLight(BaseLight base, vec3 normal, vec3 lightDirection, vec3 viewDirection);
vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);

float near = 0.1;
float far = 100.0;
// Utility method to make depth buffer values linear for perspective projection
float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Convert to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    vec3 outputColor = vec3(0.0);
    vec3 norm = normalize(v_normal);
    vec3 viewDirection = normalize(viewPosition - v_fragPos);

    outputColor += calculateDirLight(dirLight, norm, viewDirection);
    // TODO: add more point lightss
    outputColor += calculatePointLight(pointLight, norm, v_fragPos, viewDirection);
    outputColor += calculateSpotLight(spotLight, norm, v_fragPos, viewDirection);

    color = vec4(outputColor, 1.0);
    // To visualise Depth Buffer:
    //color = vec4(vec3(gl_FragCoord.z), 1.0);
    // Linear Depth buffer values
    //color = vec4(vec3(linearizeDepth(gl_FragCoord.z) / far), 1.0);
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

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fragPos);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    BaseLight newLight = calculateLight(light.base, normal, lightDirection, viewDirection);

    newLight.ambient *= attenuation;
    newLight.diffuse *= attenuation;
    newLight.specular *= attenuation;
    return (newLight.ambient + newLight.diffuse + newLight.specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fragPos);
    float theta = dot(lightDirection, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    BaseLight newLight = calculateLight(light.base, normal, lightDirection, viewDirection);

    newLight.diffuse *= intensity;
    newLight.specular *= intensity;
    return (newLight.ambient + newLight.diffuse + newLight.specular);
}
