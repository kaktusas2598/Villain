#include "TerrainComponent.hpp"

#include "terrain/FaultFormationTerrain.hpp"
#include "terrain/MidpointDisplacementTerrain.hpp"

namespace Villain {

    TerrainComponent::TerrainComponent(
            float scale,
            float texScale,
            std::vector<std::string> textureFilenames) :
        scale(scale), texScale(texScale), textureFilenames(textureFilenames)
    {
        VILLAIN_SET_COMPONENT_ID(TerrainComponent);
    }


    void TerrainComponent::createHeightmapTerrain(const std::string& binaryHeightMapFilename) {
        terrain = new Terrain();
        type = TerrainType::BASIC;

        terrain->init(scale, texScale, textureFilenames);
        terrain->loadFromFile(binaryHeightMapFilename);
    }

    void TerrainComponent::createFaultFormationTerrain(int size, int iterations, float minHt, float maxHt, float filter, int patch) {
        terrain = new FaultFormationTerrain();
        type = TerrainType::FAULT_FORMATION;

        terrain->init(scale, texScale, textureFilenames);
        ((FaultFormationTerrain*)terrain)->createFaultFormation(size, iterations, minHt, maxHt, filter);
    }

    void TerrainComponent::createMidpointDisplacementTerrain(int size, int patch, float roughness, float minHt, float maxHt) {
        terrain = new MidpointDisplacementTerrain();
        type = TerrainType::MIDPOINT_DISPLACEMENT;

        terrain->init(scale, texScale, textureFilenames);
        ((MidpointDisplacementTerrain*)terrain)->createMidpointDisplacement(size, patch, roughness,  minHt, maxHt);
    }

    void TerrainComponent::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera) {
        // TODO: Lighting
        switch (type) {
            case TerrainType::BASIC:
            case TerrainType::FAULT_FORMATION:
                terrain->render(renderingEngine, &camera);
                break;
                break;
            case TerrainType::MIDPOINT_DISPLACEMENT:
                //glm::vec3 dir = -1.f * glm::normalize(glm::vec3(0.0f) - lightNode->getTransform()->getPos());
                //midpointDisplacementTerrain.setLightDirection(dir);
                terrain->render(renderingEngine, &camera);
                break;
        }
    }
}
