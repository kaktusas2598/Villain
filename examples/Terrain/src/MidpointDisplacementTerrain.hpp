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
};

#endif // __MIDPOINT_DISPLACEMENT_TERRAIN__
