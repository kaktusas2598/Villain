#include "FaultFormationTerrain.hpp"

namespace Villain {

    void FaultFormationTerrain::createFaultFormation(int size, int iterations, float minHt, float maxHt, float filter, int patch) {
        terrainSize = size;
        patchSize = patch;
        minHeight = minHt;
        maxHeight = maxHt;

        heightMap = new float[terrainSize * terrainSize];
        // Before generating faults, initialise height map to zeroes
        for (int i = 0; i < terrainSize * terrainSize; i++) {
            heightMap[i] = 0.0f;
        }

        createFaultFormationInternal(iterations, minHeight, maxHeight, filter);

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

    void FaultFormationTerrain::createFaultFormationInternal(int iterations, float minHt, float maxHt, float filter) {
        float deltaHeight = maxHt - minHt;

        for (int currIter = 0; currIter < iterations; currIter++) {
            float iterationRatio = (float)currIter / (float)iterations;
            float height = maxHt - iterationRatio * deltaHeight;

            TerrainPoint p1, p2;
            generateRandomTerrainPoints(p1, p2);

            int dirX = p2.x - p1.x;
            int dirZ = p2.z - p1.z;

            for (int z = 0; z < terrainSize; z++) {
                for (int x = 0; x < terrainSize; x++) {
                    int dirX_in = x - p1.x;
                    int dirZ_in = z - p1.z;

                    int crossProduct = dirX_in * dirZ - dirZ_in * dirX;
                    if (crossProduct > 0) {
                        float currHeight = heightMap[x * terrainSize + z];
                        heightMap[x * terrainSize + z] = currHeight + height;
                    }
                }
            }
        }

        applyFIRFilter(filter);
    }

    void FaultFormationTerrain::applyFIRFilter(float filter) {
        // 1st filter sweep: left to right
        for (int z = 0; z < terrainSize; z++) {
            float previousValue = heightMap[z];
            for (int x = 1; x < terrainSize; x++) {
                previousValue = FIRFilterSinglePoint(x, z, previousValue, filter);
            }
        }

        // 2nd filter sweep: right to left
        for (int z = 0; z < terrainSize; z++) {
            float previousValue = heightMap[(terrainSize - 1) * terrainSize + z];
            for (int x = terrainSize - 2; x >= 0; x--) {
                previousValue = FIRFilterSinglePoint(x, z, previousValue, filter);
            }
        }

        // 3rd sweep from bottom to top
        for (int x = 0; x < terrainSize; x++) {
            float previousValue = heightMap[x * terrainSize];
            for (int z = 1; z < terrainSize; z++) {
                previousValue = FIRFilterSinglePoint(x, z, previousValue, filter);
            }
        }

        // 4th sweep from top to bottom
        for (int x = 0; x < terrainSize; x++) {
            float previousValue = heightMap[x * terrainSize + terrainSize - 1];
            for (int z = terrainSize - 2; z >= 0; z--) {
                previousValue = FIRFilterSinglePoint(x, z, previousValue, filter);
            }
        }
    }

    float FaultFormationTerrain::FIRFilterSinglePoint(int x, int z, float prevVal, float filter) {
        float currValue = heightMap[x * terrainSize + z];
        // LERP from previous to new value
        float newValue = filter * prevVal + (1 - filter) * currValue;
        heightMap[x * terrainSize + z] = newValue;
        return newValue;
    }

    void FaultFormationTerrain::generateRandomTerrainPoints(TerrainPoint& p1, TerrainPoint& p2) {
        p1.x = rand() % terrainSize;
        p1.z = rand() % terrainSize;

        int counter = 0;
        do {
            p2.x = rand() % terrainSize;
            p2.z = rand() % terrainSize;

            if (counter++ == 1000) {
                printf("Endless loop while trying to generate random terrain points for fault formation.\n");
            }

        } while(p1.isEqual(p2));
    }
}
