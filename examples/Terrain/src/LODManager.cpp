#include "LODManager.hpp"
#include <cstdio>

int LODManager::initLodManager(int patchSize, int numPatchesX, int numPatchesZ, int worldScale) {
    this->patchSize = patchSize;
    this->numPatchesX = numPatchesX;
    this->numPatchesZ = numPatchesZ;
    this->worldScale = worldScale;

    calcMaxLOD();

    PatchLOD zero;
    map = new PatchLOD[numPatchesX * numPatchesZ];
    for (int i = 0; i < numPatchesX * numPatchesZ; i++) {
        map[i] = zero;
    }

    regions.resize(maxLOD + 1);
    calcLODRegions();

    return maxLOD;
}

void LODManager::update(const glm::vec3& cameraPos) {
    // Calculate the core LOD based on the distance from the camera to the center of the patch
    updateLodMapPass1(cameraPos);
    // Match the ring LOD of every patch to the core LOD of its neighbours
    updateLodMapPass2(cameraPos);
}

void LODManager::updateLodMapPass1(const glm::vec3& cameraPos) {
    int centerStep = patchSize / 2;

    for (int lodMapZ = 0; lodMapZ < numPatchesZ; lodMapZ++) {
        for (int lodMapX = 0; lodMapX < numPatchesX; lodMapX++) {
            int x = lodMapX * (patchSize - 1) + centerStep;
            int z = lodMapZ * (patchSize - 1) + centerStep;

            glm::vec3 patchCenter = glm::vec3(x * worldScale, 0.0f, z * worldScale);
            float distanceToCamera = glm::distance(cameraPos, patchCenter);
            int coreLod = distanceToLOD(distanceToCamera);

            map[lodMapX * numPatchesZ + lodMapZ].Core = coreLod;
        }
    }
}

void LODManager::updateLodMapPass2(const glm::vec3& cameraPos) {
    int step = patchSize / 2;

    for (int lodMapZ = 0; lodMapZ < numPatchesZ; lodMapZ++) {
        for (int lodMapX = 0; lodMapX < numPatchesX; lodMapX++) {
            int coreLod = map[lodMapX * numPatchesZ + lodMapZ].Core;

            int indexLeft = lodMapX;
            int indexRight = lodMapX;
            int indexTop = lodMapZ;
            int indexBottom = lodMapZ;

            if (lodMapX > 0) {
                indexLeft--;

                if (map[indexLeft * numPatchesZ + lodMapZ].Core > coreLod) {
                    map[lodMapX * numPatchesZ + lodMapZ].Left = 1;
                } else {
                    map[lodMapX * numPatchesZ + lodMapZ].Left = 0;
                }
            }

            if (lodMapX < numPatchesX - 1) {
                indexRight++;

                if (map[indexRight * numPatchesZ + lodMapZ].Core > coreLod) {
                    map[lodMapX * numPatchesZ + lodMapZ].Right = 1;
                } else {
                    map[lodMapX * numPatchesZ + lodMapZ].Right = 0;
                }
            }

            if (lodMapZ > 0) {
                indexBottom--;

                if (map[lodMapX * numPatchesZ + indexBottom].Core > coreLod) {
                    map[lodMapX * numPatchesZ + lodMapZ].Bottom = 1;
                } else {
                    map[lodMapX * numPatchesZ + lodMapZ].Bottom = 0;
                }
            }

            if (lodMapZ < numPatchesZ - 1) {
                indexTop++;

                if (map[lodMapX * numPatchesZ + indexTop].Core > coreLod) {
                    map[lodMapX * numPatchesZ + lodMapZ].Top = 1;
                } else {
                    map[lodMapX * numPatchesZ + lodMapZ].Top = 0;
                }
            }
        }
    }
}

const LODManager::PatchLOD& LODManager::getPatchLOD(int patchX, int patchZ) const {
    return map[patchX * numPatchesZ + patchZ];
}

void LODManager::printLODMap() {
    for (int lodMapZ = numPatchesZ; lodMapZ >= 0; lodMapZ--) {
        printf("%d: ", lodMapZ);
        for (int lodMapX = 0; lodMapX < numPatchesX; lodMapX++) {
            printf("%d ", map[lodMapX * numPatchesZ + lodMapZ].Core);
        }
    }
}

void LODManager::calcLODRegions() {
    int sum = 0;

    for (int i = 0; i <= maxLOD; i++) {
        sum += (i + 1);
    }

    const float Z_FAR = 5000.0f;
    // Range for LOD0
    float X = Z_FAR / (float)sum;

    int temp = 0;
    for (int i = 0; i <= maxLOD; i++) {
        int curRange = (int)(X * (i + 1));
        regions[i] = temp + curRange;
        temp += curRange;
    }
}

void LODManager::calcMaxLOD() {
    int numSegments = patchSize - 1;
    if (ceilf(log2f((float)numSegments)) != floorf(log2f((float)numSegments))) {
        printf("The number of vertices in a patch minus one needs to be a power of two\n");
        exit(0);
    }

    int patchSizeLog2 = (int)log2f((float)numSegments);
    printf("log2 of patch size %d is %d\n", patchSize, patchSizeLog2);
    maxLOD = patchSizeLog2 - 1;
}

int LODManager::distanceToLOD(float distance) {
    int lod = maxLOD;

    for (int i = 0; i <= maxLOD; i++) {
        if (distance < regions[i]) {
            lod = i;
            break;
        }
    }

    return lod;
}

