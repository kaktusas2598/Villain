#include "Game.hpp"

#include "ResourceManager.hpp"
#include "TextureGenerator.hpp"
#include "components/CameraComponent.hpp"
#include "components/LookController.hpp"
#include "components/MoveController.hpp"
#include "rendering/DebugRenderer.hpp"

using namespace Villain;

void Game::init() {
    camera = new Camera();
    camera->setZPlanes(0.1f, 4000.f); // for bigger render range
    camera->rescale(Engine::getScreenWidth(), Engine::getScreenHeight());

    debugRenderer.init();

    // Add camera
    SceneNode* cam = (new SceneNode("Free look camera", glm::vec3(0.0, 400.0, 10.0)))
            ->addComponent(new CameraComponent(camera))
            ->addComponent(new MoveController())
            ->addComponent(new LookController());
    addToScene(cam);

    // Basic terrain loaded from heightmap
    baseTerrain.init(4.0f);
    baseTerrain.loadFromFile("assets/textures/heightmap.save");
    baseTerrain.setTerrainTexture(ResourceManager::Instance()->loadTexture("assets/textures/rock01.jpg", "rock01"));

    // Terrain generated using fault formation algorithm
    faultFormationTerrain.init(4.0f);
    // FIR filter value - lower values will give more jagged edges, while higher values will produce smoother terrain
    float filter = 0.5f;
    faultFormationTerrain.createFaultFormation(256, 500, 0.0f, 300.0f, filter);
    faultFormationTerrain.setTerrainTexture(ResourceManager::Instance()->getTexture("rock01"));

    midpointDisplacementTerrain.init(4.0f);
    midpointDisplacementTerrain.createMidpointDisplacement(256, 1.0f, 0.0f, 300.0f);
    //midpointDisplacementTerrain.setTerrainTexture(ResourceManager::Instance()->getTexture("rock01"));
    TextureGenerator terrainTextureGenerator;
    //terrainTextureGenerator.loadTile("assets/textures/test.jpg");
    terrainTextureGenerator.loadTile("assets/textures/rock01.jpg");
    terrainTextureGenerator.loadTile("assets/textures/grass1.jpg");
    terrainTextureGenerator.loadTile("assets/textures/water.png");

    terrainGeneratedTexture = terrainTextureGenerator.generateTexture(1024, &faultFormationTerrain, 0.0f, 300.0f);
    midpointDisplacementTerrain.setTerrainTexture(terrainGeneratedTexture);

    ////////////////////////
    int BPP = 3;
    int textureBytes = midpointDisplacementTerrain.getSize() * midpointDisplacementTerrain.getSize() * BPP;
    unsigned char* textureData = (unsigned char*)malloc(textureBytes);
    unsigned char* ptr = textureData;


    for (int y = 0; y < midpointDisplacementTerrain.getSize(); y++) {
        for (int x = 0; x < midpointDisplacementTerrain.getSize(); x++) {

            float height = midpointDisplacementTerrain.getHeight(x, y);
            //printf("Height: %f\n", height);

            float deltaHeight = midpointDisplacementTerrain.getMaxHeight() - midpointDisplacementTerrain.getMinHeight();
            float heightRatio = (height - midpointDisplacementTerrain.getMinHeight()) / deltaHeight;

            // Map color component from 0.2 to 1.0 so that the lowest parts are not completely black
            float c = heightRatio * 0.8 + 0.2;
            glm::vec3 heightMapColor = glm::vec3(c * 255.f);

            ptr[0] = (unsigned char)heightMapColor.r;
            ptr[1] = (unsigned char)heightMapColor.g;
            ptr[2] = (unsigned char)heightMapColor.b;
            //printf("X: %d Y: %d RGB: %f %f %f\n", x, y, heightMapColor.r, heightMapColor.g, heightMapColor.b);

            ptr += BPP;
        }
    }

    heightMapDebugTexture = new Villain::Texture(GL_TEXTURE_2D);
    heightMapDebugTexture->init(midpointDisplacementTerrain.getSize(), midpointDisplacementTerrain.getSize(), 3, textureData);
    free(textureData);
}

void Game::onAppPreUpdate(float dt) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }
}

void Game::onAppRender(float dt) {
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjMatrix();

    // Draw coordinate gizmo
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(5.f, 0.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    debugRenderer.end();
    debugRenderer.render(projection * view, 1.0f);

    if (terrainType == 1)
        faultFormationTerrain.render(camera);
    else if (terrainType == 2)
        midpointDisplacementTerrain.render(camera);
    else
        baseTerrain.render(camera);
}


void Game::onAppImGuiRender(float deltaTime) {
    ImGui::Begin("Terrain Settings");
    ImGui::Text("Terrain type: ");
    ImGui::RadioButton("From height map texture", &terrainType, 0);
    ImGui::RadioButton("Generated using fault formation", &terrainType, 1);
    ImGui::RadioButton("Generated using midpoint displacement", &terrainType, 2);
    ImGui::End();

    ImGui::Begin("Generated terrain texture");
    ImGui::Image(
        (ImTextureID)terrainGeneratedTexture->getID(),
        ImGui::GetContentRegionAvail(), // will squish image to fit it in
        ImVec2(0, 1),
        ImVec2(1, 0)
        );
    ImGui::End();

    ImGui::Begin("Terrain height map");
    ImGui::Image(
        (ImTextureID)heightMapDebugTexture->getID(),
        ImGui::GetContentRegionAvail(), // will squish image to fit it in
        ImVec2(0, 1),
        ImVec2(1, 0)
        );
    ImGui::End();
}
