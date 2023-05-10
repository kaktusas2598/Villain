#include "MidpointDisplacementTerrain.hpp"

#include <random>

void MidpointDisplacementTerrain::createMidpointDisplacement(int size, float roughness, float minHt, float maxHt) {
    terrainSize = size;
    minHeight = minHt;
    maxHeight = maxHt;

    heightMap = new float[terrainSize * terrainSize];
    // Before generating faults, initialise height map to zeroes
    for (int i = 0; i < terrainSize * terrainSize; i++) {
        heightMap[i] = 0.0f;
    }

    createMidpointDisplacementF32(roughness);

    // Normalize height map here to ensure after fault formation iterations have taken place, it's
    // clamped between minHeight and maxHeight
    // First find current limits of fault formation generated terrain
    float max, min;
    max = min = heightMap[0];
    for (int i = 0; i < terrainSize * terrainSize; i++) {
        if (heightMap[i] > max)
            max = heightMap[i];
        if (heightMap[i] < min)
            min = heightMap[i];
    }

    float minMaxDelta = max - min;
    float minMaxRange = maxHeight - minHeight;

    // Normalization
    for (int i = 0; i < terrainSize * terrainSize; i++) {
        heightMap[i] = ((heightMap[i] - min) / minMaxDelta) * minMaxRange + minHeight;
    }

    triangleList.createTriangleList(terrainSize, terrainSize, this);
}

void MidpointDisplacementTerrain::createMidpointDisplacementF32(float roughness) {
    int rectSize = terrainSize;
    float currentHeight = (float)rectSize / 2.0f;
    float heightReduce = pow(2.0f, - roughness);

    while (rectSize > 0) {
        diamondStep(rectSize, currentHeight);

        squareStep(rectSize, currentHeight);

        rectSize /= 2;
        currentHeight *= heightReduce;
    }
}

void MidpointDisplacementTerrain::diamondStep(int rectSize, float currentHeight) {
    int halfRectSize = rectSize / 2;

    std::mt19937 rndEngine((unsigned int)time(nullptr));
    std::uniform_real_distribution<float> randDist(-currentHeight, currentHeight);

    for (int y = 0; y < terrainSize; y += rectSize) {
        for (int x = 0; x < terrainSize; x += rectSize) {
            int nextX = (x + rectSize) % terrainSize;
            int nextY = (y + rectSize) % terrainSize;

            float topLeft = heightMap[x * terrainSize + y];
            float topRight = heightMap[nextX * terrainSize + y];
            float bottomLeft = heightMap[x * terrainSize + nextY];
            float bottomRight = heightMap[nextX * terrainSize + nextY];

            int midX = x + halfRectSize;
            int midY = y + halfRectSize;

            float randValue = randDist(rndEngine);
            float midPoint = (topLeft + topRight + bottomLeft + bottomRight) / 4.0f;

            heightMap[midX * terrainSize + midY] = midPoint + randValue;
        }
    }
}

void MidpointDisplacementTerrain::squareStep(int rectSize, float currentHeight) {
    int halfRectSize = rectSize / 2;

    std::mt19937 rndEngine((unsigned int)time(nullptr));
    std::uniform_real_distribution<float> randDist(-currentHeight, currentHeight);

    for (int y = 0; y < terrainSize; y += rectSize) {
        for (int x = 0; x < terrainSize; x += rectSize) {
            int nextX = (x + rectSize) % terrainSize;
            int nextY = (y + rectSize) % terrainSize;

            int midX = x + halfRectSize;
            int midY = y + halfRectSize;

            int prevMidX = (x - halfRectSize + terrainSize) % terrainSize;
            int prevMidY = (y - halfRectSize + terrainSize) % terrainSize;

            float currTopLeft = heightMap[x * terrainSize + y];
            float currTopRight = heightMap[nextX * terrainSize + y];
            float currCenter = heightMap[midX * terrainSize + midY];
            float prevYCenter = heightMap[midX * terrainSize + prevMidY];
            float currBottomLeft = heightMap[x * terrainSize + nextY];
            float prevXCenter = heightMap[prevMidX * terrainSize + midY];


            float currLeftMid = (currTopLeft + currCenter + currBottomLeft + prevXCenter) / 4.0f + randDist(rndEngine);
            float currTopMid = (currTopLeft + currCenter + currTopLeft + prevYCenter) / 4.0f + randDist(rndEngine);

            heightMap[midX * terrainSize + y] = currTopMid;
            heightMap[x * terrainSize + midY] = currLeftMid;
        }
    }

}

