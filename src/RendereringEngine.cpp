#include "RendereringEngine.hpp"

#include "Camera2D.hpp"
#include "Camera3D.hpp"
#include "Engine.hpp"
#include "InputManager.hpp"
#include "Mesh.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"

namespace Villain {

    RenderingEngine::RenderingEngine() {

        defaultShader = new Shader();
        defaultShader->createFromResource("forward-ambient");

        mainCamera = new Camera3D();
    }

    void RenderingEngine::render(SceneNode* node) {
        node->render(defaultShader, this, mainCamera);


        // Loop through lights now
        // Render all lights, generate shadow maps, etc
        for (auto& light: lights) {

        }

    }
}
