#include "RendereringEngine.hpp"

#include "Engine.hpp"
#include "InputManager.hpp"
#include "Mesh.hpp"
#include "MeshUtils.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"

namespace Villain {

    std::map<std::string, unsigned int> RenderingEngine::samplerMap;

    RenderingEngine::RenderingEngine(Engine* e): engine(e) {

        defaultShader = Shader::createFromResource("forward-ambient");
        dirShadowMapShader = Shader::createFromResource("shadowMap");
        omnidirShadowMapShader = Shader::createFromResource("shadowCubeMap");

        mainCamera = new Camera(ProjectionType::PERSPECTIVE);
        altCamera = new Camera(ProjectionType::NONE);

        samplerMap.emplace("diffuse", 0);
        samplerMap.emplace("specular", 1);
        samplerMap.emplace("normal", 2);
        samplerMap.emplace("disp", 3);
        samplerMap.emplace("shadow", 4);

        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);

        screenQuad = MeshUtils<VertexP1N1UV>::getXYPlane(glm::vec3(0.0f), glm::vec2(1.0f), new float[4]{0.0, 1.0, 0.0, 1.0});
        // TODO: refactor hardcoded shadow map size
        shadowBuffer = new FrameBuffer(1024, 1024, 1, new GLenum[1]{GL_DEPTH_ATTACHMENT});
        omniShadowBuffer = new FrameBuffer(1024, 1024, 1, new GLenum[1]{GL_DEPTH_ATTACHMENT}, true);
        mirrorBuffer = new FrameBuffer(e->getScreenWidth(), e->getScreenHeight(), 1, new GLenum[1]{GL_COLOR_ATTACHMENT0});
    }

    RenderingEngine::~RenderingEngine() {
        delete defaultShader;
        delete mainCamera;
        delete altCamera;
        delete screenQuad;
        delete shadowBuffer;
        delete mirrorBuffer;
    }

    void RenderingEngine::render(SceneNode* node) {
        // 1st Rendering Pass: Render to ambient scene to mirror buffer for rear view mirror effect
        mirrorBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Camera mirrorCamera = *mainCamera;
        mirrorCamera.setPosition(mainCamera->getPosition());
        // Rotates camera
        glm::vec3 rot = mirrorCamera.getRotation();
        rot.y += 180.0f;
        mirrorCamera.setRotation(rot);

        defaultShader->bind();
        defaultShader->setUniformVec3("color", ambientLight);
        activeLight = nullptr;
        node->render(defaultShader, this, &mirrorCamera);
        //////////

        // 2nd Rendering Pass: main pass
        bindMainTarget();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        defaultShader->bind();
        defaultShader->setUniformVec3("color", ambientLight);
        activeLight = nullptr;
        node->render(defaultShader, this, mainCamera);

        // Loop through lights now
        // Render all lights, generate shadow maps, etc
        for (auto& light: lights) {
            activeLight = light;

            // Shadow map Render Pass
            ShadowInfo* shadowInfo = activeLight->getShadowInfo();
            if (shadowInfo) {
                Shader* shadowMapShader;
                if (activeLight->type() == "point") {
                    shadowMapShader = omnidirShadowMapShader;
                    // OmniDirectional Shadow Mapping
                    glViewport(0, 0, 1024, 1024);
                    omniShadowBuffer->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);
                    glm::vec3 lightPos = activeLight->GetTransform()->getPos();
                    glm::mat4 shadowProj = shadowInfo->getProjection();

                    // Light space transform for each of 6 faces around Point light source, used by geometry shader
                    std::vector<glm::mat4> shadowTransforms;
                    shadowTransforms.push_back(shadowProj *
                            glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
                    shadowTransforms.push_back(shadowProj *
                            glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
                    shadowTransforms.push_back(shadowProj *
                            glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
                    shadowTransforms.push_back(shadowProj *
                            glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
                    shadowTransforms.push_back(shadowProj *
                            glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
                    shadowTransforms.push_back(shadowProj *
                            glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));

                    shadowMapShader->bind();
                    shadowMapShader->setUniformVec3("lightPos", activeLight->GetTransform()->getPos());
                    shadowMapShader->setUniform1f("farPlane", shadowInfo->getFarPlane());
                    for (unsigned int i = 0; i < 6; ++i)
                        shadowMapShader->setUniformMat4f("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

                } else {
                    shadowMapShader = dirShadowMapShader;
                    // Directional Shadow Mapping
                    // Render shadows for each light into depth map and go back to main pass
                    glViewport(0, 0, 1024, 1024);
                    shadowBuffer->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);
                    altCamera->setPosition(activeLight->GetTransform()->getPos());

                    glm::mat4 lightView;
                    if (activeLight->type() == "directional") {
                        //DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(activeLight);
                        //glm::mat4 lightView =  glm::lookAt(activeLight->GetTransform()->getPos(), dirLight->Direction, glm::vec3(0.0, 1.0, 0.0));
                        lightView =  glm::lookAt(activeLight->GetTransform()->getPos(), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
                    } else if (activeLight->type() == "spot") {
                        SpotLight* spotLight = (SpotLight*)(activeLight);
                        lightView =  glm::lookAt(spotLight->Position, spotLight->Position + spotLight->Direction, glm::vec3(0.0, 1.0, 0.0));
                    }

                    lightMatrix = shadowInfo->getProjection() * lightView;

                    shadowMapShader->bind();
                    shadowMapShader->setUniformMat4f("lightMatrix", lightMatrix);
                }

                // For shadow map reverse culling to try and solve peter panning problem
                if (shadowInfo->getFlipFaces()) glCullFace(GL_FRONT);
                // Render scene to shadow map
                frustumCullingEnabled = false; // NOTE: Frustum culling system not properly implemented and won't work for shadow mapping
                // NOTE: Will need to fix shader uniform logic inside mesh and model renderer so they only set what's needed
                activeLight = nullptr; // Disabling active light while rendering to shadow map, to avoid trying to set light uniforms
                node->render(shadowMapShader, this, altCamera);
                activeLight = light;
                frustumCullingEnabled = true;
                // Revert culling back to normal behaviour
                if (shadowInfo->getFlipFaces()) glCullFace(GL_BACK);
            }

            // Main lighting pass
            bindMainTarget();

            //// Using additive blending here to render lights one by one and blend onto the scene
            //// this is so called forward multi-pass rendering
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            light->getShader()->bind();
            if (shadowInfo) {
                if (activeLight->type() == "point") {
                    omniShadowBuffer->getTexture()->bind(getSamplerSlot("shadow"));
                    light->getShader()->setUniform1i("shadowCubeMap", getSamplerSlot("shadow"));
                    light->getShader()->setUniform1f("farPlane", shadowInfo->getFarPlane());

                } else {
                    shadowBuffer->getTexture()->bind(getSamplerSlot("shadow"));
                    light->getShader()->setUniform1i("shadowMap", getSamplerSlot("shadow"));
                    light->getShader()->setUniformMat4f("lightMatrix", lightMatrix);
                }
                light->getShader()->setUniform1f("shadowBias", shadowInfo->getBias()/1024.f);
            }
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
        activeLight = nullptr;
        // Smaller render target on top
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Material mirrorMat{"null", mirrorBuffer->getTexture(), 1};
        //Material mirrorMat{"null", shadowBuffer->getTexture(), 1};
        planeTransform.setScale(0.25);
        planeTransform.setPos(glm::vec3(0.75f, 0.75f, -0.3f));
        planeTransform.setEulerRot(0.0, 180.0, 90.0);
        frustumCullingEnabled = false;
        defaultShader->updateUniforms(planeTransform, mirrorMat, *this, *altCamera);
        frustumCullingEnabled = true;
        defaultShader->setUniformVec3("color", ambientLight);
        screenQuad->draw(*defaultShader, mirrorMat);
    }

    void RenderingEngine::bindMainTarget() {
        // Will cause framebuffer in editor to be unbound
        if (engine->editModeActive()) {
            engine->getSceneBuffer()->bind();
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, Engine::getScreenWidth(), Engine::getScreenHeight());
        }
    }
}
