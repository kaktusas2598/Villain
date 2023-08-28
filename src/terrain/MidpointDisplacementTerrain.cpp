#include "MidpointDisplacementTerrain.hpp"

#include <random>

namespace Villain {

    void MidpointDisplacementTerrain::createMidpointDisplacement(int size, int patch, float roughness, float minHt, float maxHt) {
        terrainSize = size;
        patchSize = patch;
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

        if (patch == 0) {
            triangleList.createTriangleList(terrainSize, terrainSize, this);
        } else {
            geomipGrid.createGeomipGrid(terrainSize, terrainSize, patchSize, this);
            useLOD = true;
        }
    }

    void MidpointDisplacementTerrain::createMidpointDisplacementF32(float roughness) {
        int rectSize = calculateNextPowerOf2(terrainSize);
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

                // To fix edge anomaly for when terrain size if not power of 2
                // NOTE: still wouldn't recommend using terrain sizes not power of 2 as this doesn't seem to fix it properly
                if (nextX < x) {
                    nextX = terrainSize - 1;
                }
                if (nextY < y) {
                    nextY = terrainSize - 1;
                }

                float topLeft = heightMap[x * terrainSize + y];
                float topRight = heightMap[nextX * terrainSize + y];
                float bottomLeft = heightMap[x * terrainSize + nextY];
                float bottomRight = heightMap[nextX * terrainSize + nextY];

                int midX = (x + halfRectSize) % terrainSize;
                int midY = (y + halfRectSize) % terrainSize;

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

                int midX = (x + halfRectSize) % terrainSize;
                int midY = (y + halfRectSize) % terrainSize;

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
}
