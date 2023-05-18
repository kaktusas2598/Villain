#ifndef __FAULT_FORMATION_TERRAIN__
#define __FAULT_FORMATION_TERRAIN__

#include "Terrain.hpp"

class FaultFormationTerrain : public Terrain {
    public:
        FaultFormationTerrain() {}

        void createFaultFormation(int size, int iterations, float minHt, float maxHt, float filter);

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

        void createFaultFormationInternal(int iterations, float minHt, float maxHt, float filter);
        void generateRandomTerrainPoints(TerrainPoint& p1, TerrainPoint& p2);

        // Finite Impulse Response filter
        void applyFIRFilter(float filter);
        float FIRFilterSinglePoint(int x, int z, float prevVal, float filter);
};

#endif // __FAULT_FORMATION_TERRAIN__
