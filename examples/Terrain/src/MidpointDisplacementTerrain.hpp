#ifndef __MIDPOINT_DISPLACEMENT_TERRAIN__
#define __MIDPOINT_DISPLACEMENT_TERRAIN__

#include "Terrain.hpp"

class MidpointDisplacementTerrain : public Terrain {
    public:
        MidpointDisplacementTerrain() {}

        void createMidpointDisplacement(int size, float roughness, float minHt, float maxHt);

    private:
        void createMidpointDisplacementF32(float roughness);
        void diamondStep(int rectSize, float currentHeight);
        void squareStep(int rectSize, float currentHeight);

        // NOTE: Need to move this to general math utils module in the near future
        int calculateNextPowerOf2(int x) {
            int ret = 1;

            if (x == 1)
                return 2;

            while (ret < x) {
                ret *= 2;
            }

            return ret;
        }
};

#endif // __MIDPOINT_DISPLACEMENT_TERRAIN__
