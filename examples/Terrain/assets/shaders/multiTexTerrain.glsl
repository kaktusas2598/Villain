#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

// For now no need for world/model matrix as terrain will be constructed using world coordinates and won't be rotated/translated
uniform mat4 projection;
uniform mat4 view;

uniform float minHeight;
uniform float maxHeight;

out vec4 color;
out vec2 inUV;
out vec3 worldPos;
out vec3 inNormal;

out float visibility;
uniform bool useExponentialFog = false;
// For Fog calculation using exponential formula
uniform float fogDensity = 0.035; // increasing this, increases fog thickness and decreases visibility
uniform float fogGradient = 5.0; // increasing this, sharpens switch to fog
uniform vec3 fogColor;
// For layered fog
uniform vec3 viewPosition; // Camera world position
uniform float layeredFogTop = 250.0; // Maximum height of fog
uniform float layeredFogEnd = 200.0; // Max distance?

// NOTE: beyond this would be interesting to investigate volumetric fog techniques
float calcLayeredFog() {
    vec3 cameraProj = viewPosition;
    cameraProj.y = 0.0;

    // NOTE: All mentions of position in this function should change once we provide model matrix to terrain
    vec3 pixelProj = position;
    pixelProj.y = 0.0;

    float deltaD = length(cameraProj - pixelProj) / layeredFogEnd;
    float deltaY = 0.0;
    float densityIntegral = 0.0;

    if (viewPosition.y > layeredFogTop) { // Camera is above the top of the fog
        if (position.y < layeredFogTop) { // Pixel is inside the fog
            deltaY = (layeredFogTop - position.y) / layeredFogTop;
            densityIntegral = deltaY * deltaY * 0.5f;
        }    // else, the pixel is above the fog and no change is made
    } else { // Camera is inside the fog
        if (position.y < layeredFogTop) { // Pixel is inside the fog
            deltaY = abs(viewPosition.y - position.y) / layeredFogTop;
            float deltaCamera = (layeredFogTop - viewPosition.y) / layeredFogTop;
            float densityIntegralCamera = deltaCamera * deltaCamera * 0.5f;
            float deltaPixel = (layeredFogTop - position.y) / layeredFogTop;
            float densityIntegralPixel = deltaPixel* deltaPixel * 0.5f;
            densityIntegral = abs(densityIntegralCamera - densityIntegralPixel);

        } else { // Pixel is above the fog
            deltaY = (layeredFogTop - viewPosition.y) / layeredFogTop;
            densityIntegral = deltaY * deltaY * 0.5f;
        }
    }

    float linearFogDensity = 0.0f;
    if (deltaY != 0.0) {
        linearFogDensity = (sqrt(1.0 + ((deltaD / deltaY) * (deltaD / deltaY)))) * densityIntegral;
    }

    float fogFactor = exp(-linearFogDensity);
    return fogFactor;
}

float calcExponentialFog() {
    vec4 relativeToCamera = view * vec4(position, 1.0); // Will have to change once we have model matrix for terrain
    float distance = length(relativeToCamera.xyz);

    float fogFactor = exp(-pow(distance * fogDensity, fogGradient));
    fogFactor = clamp(fogFactor, 0.1, 1.0);
    return fogFactor;
}

void main() {
    gl_Position = projection * view * vec4(position, 1.0);

    float deltaHeight = maxHeight - minHeight;
    float heightRatio = (position.y - minHeight) / deltaHeight;

    // Map color component from 0.2 to 1.0 so that the lowest parts are not completely black
    float c = heightRatio * 0.8 + 0.2;

    color = vec4(c, c, c, 1.0);
    inUV = uv;
    // NOTE: If doing any world transformations, then also need to provide world/model matrix as uniform
    worldPos = position;
    inNormal = normal;

    // FOG
    if (fogColor != vec3(0.0)) {
        if (useExponentialFog) {
            visibility = calcExponentialFog();
        } else {
            visibility = calcLayeredFog();
        }
    }
}

#shader fragment
#version 330 core

in vec4 color;
in vec2 inUV;
in vec3 worldPos;
in vec3 inNormal;
in float visibility;

uniform sampler2D textureHeight0;
uniform sampler2D textureHeight1;
uniform sampler2D textureHeight2;
uniform sampler2D textureHeight3;

uniform float height0 = 20.0;
uniform float height1 = 80.0;
uniform float height2 = 180.0;
uniform float height3 = 230.0;

uniform vec3 reverseLightDir;
uniform vec3 fogColor;

vec4 calcTexColor() {
    vec4 texColor;
    float height = worldPos.y;

    if (height < height0) {
        texColor = texture(textureHeight0, inUV);
    } else if (height < height1) {
        vec4 color0 = texture(textureHeight0, inUV);
        vec4 color1 = texture(textureHeight1, inUV);
        float delta = height1 - height0;
        float factor = (height - height0) / delta;
        texColor = mix(color0, color1, factor);
    } else if (height < height2) {
        vec4 color0 = texture(textureHeight1, inUV);
        vec4 color1 = texture(textureHeight2, inUV);
        float delta = height2 - height1;
        float factor = (height - height1) / delta;
        texColor = mix(color0, color1, factor);
    } else if (height < height3) {
        vec4 color0 = texture(textureHeight2, inUV);
        vec4 color1 = texture(textureHeight3, inUV);
        float delta = height3 - height2;
        float factor = (height - height2) / delta;
        texColor = mix(color0, color1, factor);
    } else {
        texColor = texture(textureHeight3, inUV);
    }

    return texColor;
}

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = calcTexColor();

    vec3 normal = normalize(inNormal);
    // Calculate diffuse lighting
    // NOTE: will need to integrate with multi-pass forward rendering lighting?
    float diffuse = max(dot(normal, reverseLightDir), 0.0);
    // poor man's version of ambient lighting below, providing at least some diffuse light
    diffuse = max(0.3, diffuse);

    outColor = texColor * color * diffuse;

    if (fogColor != vec3(0.0)) {
        outColor = mix(vec4(fogColor, 1.0), outColor, visibility);
    }
}
