#pragma once

#include "NodeComponent.hpp"
#include "terrain/Terrain.hpp"

namespace Villain {

    enum class TerrainType {
        BASIC, FAULT_FORMATION, MIDPOINT_DISPLACEMENT
    };

    /// Lets user create, control and render Terrain through Scene graph component driven approach
    class TerrainComponent : public NodeComponent {
        public:
            // TODO: Add Terrain to editor and xml parser/writer
            TerrainComponent(
                    float scale = 1.0f,
                    float texScale = 1.0f,
                    std::vector<std::string> textureFilenames = {});

            /// Basic heightmap terrain named constructor
            void createHeightmapTerrain(const std::string& binaryHeightMapFilename);
            /// Fault formation terrain named constructor
            void createFaultFormationTerrain(int size, int iterations, float minHt, float maxHt, float filter, int patch = 0);
            /// Midpoint displacement terrain named constructor
            void createMidpointDisplacementTerrain(int size, int patch, float roughness, float minHt, float maxHt);

            ~TerrainComponent() {
                delete terrain;
            }

            virtual void render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);

            Terrain* getTerrain() { return terrain; }
            TerrainType getType() const { return type;}
        private:
            Terrain* terrain;
            TerrainType type;

            float scale = 1.0f;
            float texScale = 1.0f;
            std::vector<std::string> textureFilenames = {};
    };
}
