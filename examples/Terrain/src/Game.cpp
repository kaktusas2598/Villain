#include "Game.hpp"

#include "components/CameraComponent.hpp"
#include "components/LookController.hpp"
#include "components/MoveController.hpp"
#include "rendering/DebugRenderer.hpp"

using namespace Villain;

void Game::init() {
    camera = new Camera();
    camera->setZPlanes(0.1f, 1000.f); // for bigger render range
    camera->rescale(Engine::getScreenWidth(), Engine::getScreenHeight());

    debugRenderer.init();

    // Add camera
    SceneNode* cam = (new SceneNode("Free look camera", glm::vec3(0.0, 0.0, 10.0)))
            ->addComponent(new CameraComponent(camera))
            ->addComponent(new MoveController())
            ->addComponent(new LookController());
    addToScene(cam);
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
}
