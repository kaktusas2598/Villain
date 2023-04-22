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

        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
    }

    RenderingEngine::~RenderingEngine() {
        delete defaultShader;
    }

    void RenderingEngine::render(SceneNode* node) {
        defaultShader->bind();
        // NOTE: Commented out for now, because there needs to be a nice way to set it somewhere
        defaultShader->setUniformVec3("color", ambientLight);
        activeLight = nullptr;
        node->render(defaultShader, this, mainCamera);

        // Loop through lights now
        // Render all lights, generate shadow maps, etc
        for (auto& light: lights) {
            activeLight = light;

            // HACK: for now all spot lights will act as a flashlight
            // and use main camera's position
            if (activeLight->type() == "spot") {
                SpotLight* flashlight = dynamic_cast<SpotLight*>(activeLight);
                flashlight->Position = mainCamera->getPosition();
                flashlight->Direction = dynamic_cast<Camera3D*>(mainCamera)->getFront();
            }

            //// NOTE: shadow mapping techniques will be here

            //// Using additive blending here to render lights one by one and blend onto the scene
            //// this is so called forward multi-pass rendering
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            node->render(light->getShader(), this, mainCamera);

            // Reset to default blending
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }

    }
}
