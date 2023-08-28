#pragma once

#include "glm/glm.hpp"
#include <vector>

namespace Villain {

    // Selection of LOD permutation for a patch, max LOD calculation
    class LODManager {
        public:
            int initLodManager(int patch, int patchesX, int patchesZ, int worldSc);

            void update(const glm::vec3& cameraPos);

            struct PatchLOD {
                int Core = 0;
                int Left = 0;
                int Right = 0;
                int Top = 0;
                int Bottom = 0;
            };

            const PatchLOD& getPatchLOD(int patchX, int patchZ) const;

            void printLODMap();

        private:
            void calcLODRegions();
            void calcMaxLOD();
            void updateLodMapPass1(const glm::vec3& cameraPos);
            void updateLodMapPass2(const glm::vec3& cameraPos);
            int distanceToLOD(float distance);

            int maxLOD = 0;
            int numPatchesX = 0;
            int numPatchesZ = 0;
            int patchSize = 0;
            float worldScale = 0.0f;

            PatchLOD* map;
            std::vector<int> regions;
    };
}
