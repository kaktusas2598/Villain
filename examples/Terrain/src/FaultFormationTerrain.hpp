#ifndef __FAULT_FORMATION_TERRAIN__
#define __FAULT_FORMATION_TERRAIN__

#include "Terrain.hpp"

class FaultFormationTerrain : public Terrain {
    public:
        FaultFormationTerrain() {}

        void createFaultFormation(int size, int iterations, float minHt, float maxHt);

    private:
        struct TerrainPoint {
            int x = 0;
            int z = 0;

            void print() {
                printf("[%d, %d]", x, z);
            }

            bool isEqual(TerrainPoint& p) {
                return ((x == p.x) && (z == p.z));
            }
        };

        void createFaultFormationInternal(int iterations, float minHt, float maxHt);
        void generateRandomTerrainPoints(TerrainPoint& p1, TerrainPoint& p2);
};

#endif // __FAULT_FORMATION_TERRAIN__
