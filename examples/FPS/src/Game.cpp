#include "Game.hpp"

#include "ResourceManager.hpp"
#include "Light.hpp"
#include "SceneNode.hpp"
#include "components/CameraComponent.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/PhysicsObjectComponent.hpp"
#include "physics/BoundingSphere.hpp"

#include "Bitmap.hpp"

using namespace Villain;

const float ROOM_WIDTH = 1.0f;
const float ROOM_LENGTH = 1.0f;
const float ROOM_HEIGHT = 2.0f;
const int NUM_TEX_EXP = 4;
const int NUM_TEXTURES = (int)pow(2, NUM_TEX_EXP);

void Game::init() {
    camera.setZPlanes(0.1f, 1000.f); // for bigger render range
    camera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());

    ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "crate");

    // Add camera
    addToScene((new SceneNode("Main camera"))->addComponent(new CameraComponent(&camera)));

    SceneNode* wall = (new SceneNode("wall", glm::vec3(4.f, 1.f, 0.f)))->addComponent(new ModelRenderer("assets/models/wall.obj"));
    wall->getTransform()->setEulerRot(0.0f, 0.f, 90.f);
    addToScene(wall);

    // Light test
    //SceneNode* dirLight = ((new SceneNode("Directional Light 1"))
                //->addComponent(new DirectionalLight(glm::vec3(0.5f), glm::vec3(0.2f), glm::vec3(1.0f),glm::vec3(-0.2f, -1.0f, -0.3f))));
    //addToScene(dirLight);

    glm::vec3 redLight = glm::vec3(1.0f, 0.0f, 0.f);
    SceneNode* pointLight = ((new SceneNode("Point Light 1", glm::vec3(4.f, 2.f, 3.f)))
                ->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f),glm::vec3(100.0f, 2.0f, -10.0f), 1.0f, 0.022f, 0.0019f)));
    addToScene(pointLight);

    SceneNode* pointLight2 = ((new SceneNode("Point Light 2"))
                ->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.022f, 0.0019f)));
    addToScene(pointLight2);

    glm::vec3 lightColor = glm::vec3(0.5f, 0.7f, 0.4f);
    SceneNode* spotLight = ((new SceneNode("Spot Light"))
                ->addComponent(new SpotLight(lightColor * glm::vec3(0.2f), lightColor, glm::vec3(1.0f), glm::vec3(20.f, 20.f, 10.f), glm::vec3(0.0f, -5.f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))/*, &camera*/)));
    addToScene(spotLight);


    // Physics demo
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(PhysicsObject(new BoundingSphere(glm::vec3(-50.0f, 4.5f, 0.f), 1.0f), glm::vec3(3.0f, 0.f, 0.f)));
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(PhysicsObject(new BoundingSphere(glm::vec3(50.0f, 5.0f, 0.f), 1.0f), glm::vec3(-3.0f, 0.f, 0.f)));

    addToScene((new SceneNode("physics object 0"))
        ->addComponent(new PhysicsObjectComponent(&getRootNode()->getEngine()->getPhysicsEngine()->getObject(0)))
        ->addComponent(new ModelRenderer("assets/models/wall.obj")));
    addToScene((new SceneNode("physics object 1"))
        ->addComponent(new PhysicsObjectComponent(&getRootNode()->getEngine()->getPhysicsEngine()->getObject(1)))
        ->addComponent(new ModelRenderer("assets/models/wall.obj")));

    // Load and generate 3D map from bitmap texture
    std::vector<VertexP1N1UV> floorVertices;
    std::vector<unsigned int> floorIndices;
    Bitmap map1("assets/textures/level1.png");
    for (int i = 0; i < map1.getWidth(); i++) {
        for (int j = 0; j < map1.getHeight(); j++) {
            Pixel pixel = map1.getPixel(i, j);

            // Selects tile from atlas using green component of the pixel in current position in bitmap
            // For walls we use red component
            // So for 16 textures, we divide color component 0 -> 255 in 16 parts
            int texX = pixel.G / NUM_TEXTURES;
            int texY = texX % NUM_TEX_EXP;
            texX /= NUM_TEX_EXP;

            float xHigher = 1.f - (float)texX / (float)NUM_TEX_EXP;
            float xLower = xHigher - 1.f / NUM_TEX_EXP;
            float yHigher = 1.f - (float)texY / (float)NUM_TEX_EXP;
            float yLower = yHigher - 1.f / NUM_TEX_EXP;

            if (pixel.R != 0 || pixel.G != 0 || pixel.B != 0) {
                // Floor vertices, normals are defaults just to make shader work
                floorIndices.push_back(floorVertices.size() + 2);
                floorIndices.push_back(floorVertices.size() + 1);
                floorIndices.push_back(floorVertices.size() + 0);
                floorIndices.push_back(floorVertices.size() + 3);
                floorIndices.push_back(floorVertices.size() + 2);
                floorIndices.push_back(floorVertices.size() + 0);

                floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), 0.0f, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yLower)});
                floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), 0.0f, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yLower)});
                floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), 0.0f, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yHigher)});
                floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), 0.0f, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yHigher)});

                // Ceilings, also revert indices to make sure textures are drawn from the bottom
                floorIndices.push_back(floorVertices.size() + 0);
                floorIndices.push_back(floorVertices.size() + 1);
                floorIndices.push_back(floorVertices.size() + 2);
                floorIndices.push_back(floorVertices.size() + 0);
                floorIndices.push_back(floorVertices.size() + 2);
                floorIndices.push_back(floorVertices.size() + 3);

                floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yLower)});
                floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yLower)});
                floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yHigher)});
                floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yHigher)});

                int texX = pixel.R / NUM_TEXTURES;
                int texY = texX % NUM_TEX_EXP;
                texX /= NUM_TEX_EXP;
                float xHigher = 1.f - (float)texX / (float)NUM_TEX_EXP;
                float xLower = xHigher - 1.f / NUM_TEX_EXP;
                float yHigher = 1.f - (float)texY / (float)NUM_TEX_EXP;
                float yLower = yHigher - 1.f / NUM_TEX_EXP;

                // Genertate walls
                Pixel pixel = map1.getPixel(i, j - 1); // get adjacent pixel
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    // Generate left wall
                    floorIndices.push_back(floorVertices.size() + 0);
                    floorIndices.push_back(floorVertices.size() + 1);
                    floorIndices.push_back(floorVertices.size() + 2);
                    floorIndices.push_back(floorVertices.size() + 0);
                    floorIndices.push_back(floorVertices.size() + 2);
                    floorIndices.push_back(floorVertices.size() + 3);

                    floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), 0.0f, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yLower)});
                    floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), 0.0f, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yLower)});
                    floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yHigher)});
                    floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yHigher)});
                }
                pixel = map1.getPixel(i, j + 1);
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    // Generate right wall
                    floorIndices.push_back(floorVertices.size() + 2);
                    floorIndices.push_back(floorVertices.size() + 1);
                    floorIndices.push_back(floorVertices.size() + 0);
                    floorIndices.push_back(floorVertices.size() + 3);
                    floorIndices.push_back(floorVertices.size() + 2);
                    floorIndices.push_back(floorVertices.size() + 0);

                    floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), 0.0f, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yLower)});
                    floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), 0.0f, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yLower)});
                    floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yHigher)});
                    floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yHigher)});
                }
                pixel = map1.getPixel(i - 1, j);
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    // Generate near wall
                    floorIndices.push_back(floorVertices.size() + 2);
                    floorIndices.push_back(floorVertices.size() + 1);
                    floorIndices.push_back(floorVertices.size() + 0);
                    floorIndices.push_back(floorVertices.size() + 3);
                    floorIndices.push_back(floorVertices.size() + 2);
                    floorIndices.push_back(floorVertices.size() + 0);

                    floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), 0.0f, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yLower)});
                    floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), 0.0f, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yLower)});
                    floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yHigher)});
                    floorVertices.push_back({glm::vec3((i * ROOM_WIDTH), ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yHigher)});
                }
                pixel = map1.getPixel(i + 1, j);
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    // Generate far wall
                    floorIndices.push_back(floorVertices.size() + 0);
                    floorIndices.push_back(floorVertices.size() + 1);
                    floorIndices.push_back(floorVertices.size() + 2);
                    floorIndices.push_back(floorVertices.size() + 0);
                    floorIndices.push_back(floorVertices.size() + 2);
                    floorIndices.push_back(floorVertices.size() + 3);

                    floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), 0.0f, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yLower)});
                    floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), 0.0f, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yLower)});
                    floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xHigher, yHigher)});
                    floorVertices.push_back({glm::vec3(((i + 1) * ROOM_WIDTH), ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(xLower, yHigher)});
                }


            }
        }
    }
    //std::vector<Texture*> floorTextures = {ResourceManager::Instance()->loadTexture("assets/models/textures/brickwall.jpg", "wall")};
    std::vector<Texture*> floorTextures = {ResourceManager::Instance()->loadTexture("assets/textures/WolfCollection.png", "wall")};
    Material bricks{"bricks", floorTextures, 8};
    Mesh<VertexP1N1UV>* floorMesh = new Mesh<VertexP1N1UV>(floorVertices, floorIndices, floorTextures);
    addToScene((new SceneNode("floor"))->addComponent(new MeshRenderer<VertexP1N1UV>(floorMesh, bricks)));
}

Game::~Game() {
}

void Game::onAppPreUpdate(float dt) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }
}
