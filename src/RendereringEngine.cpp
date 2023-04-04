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
    }

    RenderingEngine::~RenderingEngine() {
        delete defaultShader;
    }

    void RenderingEngine::render(SceneNode* node) {
        // TODO: this can be set in imgui, need getter/setter in RenderingEngine
        defaultShader->bind();
        defaultShader->setUniformVec3("color", glm::vec3(0.8f, 0.6f, 0.2f));
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

            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
        }

    }
}
