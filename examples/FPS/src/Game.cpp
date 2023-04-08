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

void Game::init() {
    camera.setZPlanes(0.1f, 1000.f); // for bigger render range
    camera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());

    ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "crate");
    std::vector<VertexP1N1UV> vertices;
    // nomals here are wrong cause im lazy, just wanted to get uv working cause it's 2nd vertex attrib, so need normals
    vertices.push_back({glm::vec3(-1.0f,  1.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)});
    vertices.push_back({glm::vec3(1.0f,  1.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)});
    vertices.push_back({glm::vec3(1.0f, -1.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)});
    vertices.push_back({glm::vec3(-1.0f, -1.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)});
    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    std::vector<Texture*> textures = {ResourceManager::Instance()->getTexture("crate")};
    Mesh<VertexP1N1UV>* mesh = new Mesh<VertexP1N1UV>(vertices, indices, textures);
    Material mat("wood", textures, 8);

    SceneNode* planeNode = (new SceneNode("Mesh"))->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));

    SceneNode* testHierarchy = (new SceneNode("Mesh Child", glm::vec3(0.0f, 3.0f, 0.0f)))->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));

    SceneNode* testHierarchyChild = new SceneNode("Mesh grandchild", glm::vec3(2.0f, 0.0f, 0.0f));
    testHierarchyChild->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
    //testHierarchy->addChild(testHierarchyChild);

    testHierarchy->getTransform()->setEulerRot(45.0f, 0.f, 0.f);

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
    pointLight->addChild(planeNode);
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

    // Map test
    std::vector<VertexP1N1UV> floorVertices;
    std::vector<unsigned int> floorIndices;
    Bitmap map1("assets/textures/level1.png");
    for (int i = 0; i < map1.getWidth(); i++) {
        for (int j = 0; j < map1.getHeight(); j++) {
            // TODO: create meshes here
            float xHigher = 1;
            float xLower = 0;
            float yHigher = 1;
            float yLower = 0;

            unsigned char* pixel = map1.getPixel(i, j);
            int r = (int)pixel[0];
            int g = (int)pixel[1];
            int b = (int)pixel[2];

            if (r == 255 && g == 255 && b == 255) {
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
            }
            //std::cout << "X: " << i << ", Y: " << j << "\n";
            //std::cout << "R: " << (int)pixel[0] << ", G: " << (int)pixel[1] << ", B: " << (int)pixel[2] << "\n";
        }
    }
    // Add floor to scene graph
    std::vector<Texture*> floorTextures = {ResourceManager::Instance()->loadTexture("assets/models/textures/brickwall.jpg", "wall")};
    Mesh<VertexP1N1UV>* floorMesh = new Mesh<VertexP1N1UV>(floorVertices, floorIndices, floorTextures);
    addToScene((new SceneNode("floor"))->addComponent(new MeshRenderer<VertexP1N1UV>(floorMesh, mat)));
}

Game::~Game() {
}

void Game::onAppPreUpdate(float dt) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }
}
