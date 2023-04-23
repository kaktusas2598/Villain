#include "RendereringEngine.hpp"

#include "Camera2D.hpp"
#include "Camera3D.hpp"
#include "Engine.hpp"
#include "InputManager.hpp"
#include "Mesh.hpp"
#include "MeshUtils.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"

namespace Villain {

    RenderingEngine::RenderingEngine(Engine* e): engine(e) {

        defaultShader = new Shader();
        defaultShader->createFromResource("forward-ambient");

        mainCamera = new Camera3D();
        altCamera = new Camera3D();

        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);

        Camera3D* cam3D = dynamic_cast<Camera3D*>(altCamera);
        cam3D->setNoProjection(); // Super hacky ATM
        screenQuad = MeshUtils<VertexP1N1UV>::getXYPlane(glm::vec3(0.0f), glm::vec2(1.0f), new float[4]{0.0, 1.0, 0.0, 1.0});
        // TODO: refactor hardcoded shadow map size
        shadowBuffer = new FrameBuffer(1024, 1024);
        mirrorBuffer = new FrameBuffer(e->getScreenWidth(), e->getScreenHeight());
    }

    RenderingEngine::~RenderingEngine() {
        delete defaultShader;
        delete mainCamera;
        delete altCamera;
        delete screenQuad;
        delete shadowBuffer;
    }

    void RenderingEngine::render(SceneNode* node) {
        // Render to ambient scene to mirror buffer for rear view mirror effect
        mirrorBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Camera3D* mainCam = (Camera3D*)mainCamera;
        Camera3D cam3D = *(Camera3D*)mainCamera;
            cam3D.setPosition(mainCamera->getPosition());
            // Rotates camera
            glm::vec3 rot = mainCam->getRotation();
            rot.y += 180.0f;
            cam3D.setRotation(rot);

        defaultShader->bind();
        defaultShader->setUniformVec3("color", ambientLight);
        activeLight = nullptr;
        node->render(defaultShader, this, &cam3D);

        // Will cause framebuffer in editor to be unbound
        if (engine->editModeActive()) {
            engine->getSceneBuffer()->bind();
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, Engine::getScreenWidth(), Engine::getScreenHeight());
        }

        //shadowBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        defaultShader->bind();
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

    // TEMP, render to texture test on plane, moved to different method so that
    // stuff like debug renderer and skybox also get rendered to shadow buffer in the 1st pass
    void RenderingEngine::postRender() {
        //if (engine->editModeActive()) {
            //engine->getSceneBuffer()->bind();
        //} else {
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //glViewport(0, 0, Engine::getScreenWidth(), Engine::getScreenHeight());
        //}

        //activeLight = nullptr;
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Material mat{"null", shadowBuffer->getTexture(), 1};
        //defaultShader->bind();
        //planeTransform.setScale(0.9); // Will create border around frambuffer, testing code
        //planeTransform.setEulerRot(0.0, 180.0, 90.0); // Rotate framebufer quad to fix orientation
        //planeTransform.setPos(glm::vec3(0.0f, 0.0f, -0.2f));
        //defaultShader->updateUniforms(planeTransform, mat, *this, *altCamera);
        //defaultShader->setUniformVec3("color", ambientLight);
        //screenQuad->draw(*defaultShader, mat);

        // Smaller render target on top, tried to make rear view mirror,
        // but that probably requires rendering scene again with rotated camera?
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Material mirrorMat{"null", mirrorBuffer->getTexture(), 1};
        planeTransform.setScale(0.25);
        planeTransform.setPos(glm::vec3(0.75f, 0.75f, -0.3f));
        planeTransform.setEulerRot(0.0, 180.0, 90.0);
        defaultShader->updateUniforms(planeTransform, mirrorMat, *this, *altCamera);
        defaultShader->setUniformVec3("color", ambientLight);
        screenQuad->draw(*defaultShader, mirrorMat);
    }
}
