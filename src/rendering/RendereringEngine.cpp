#include "RendereringEngine.hpp"

#include "Engine.hpp"
#include "Input.hpp"
#include "Mesh.hpp"
#include "MeshUtils.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"

namespace Villain {

    std::map<std::string, unsigned int> RenderingEngine::samplerMap;
    bool RenderingEngine::gammaCorrection = false;

    RenderingEngine::RenderingEngine(Engine* e): engine(e) {

        defaultShader = Shader::createFromResource("forward-ambient");
        postFXShader = Shader::createFromResource("postProcessing");
        normalDebugShader = Shader::createFromResource("normal-debug");
        dirShadowMapShader = Shader::createFromResource("shadowMap");
        omnidirShadowMapShader = Shader::createFromResource("shadowCubeMap");
        skyboxShader = Shader::createFromResource("cubemap");
        gaussianBlurShader = Shader::createFromResource("gaussianBlur");
        lightColorShader = Shader::createFromResource("lightColor");

        mainCamera = new Camera(CameraType::FIRST_PERSON);
        altCamera = new Camera(CameraType::NONE);

        samplerMap.emplace("diffuse", 0);
        samplerMap.emplace("specular", 1);
        samplerMap.emplace("normal", 2);
        samplerMap.emplace("disp", 3);
        samplerMap.emplace("shadow", 4);

        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        // Enable MSAA, number of samples set in Window class
        // Implementation would be different in Deffered renderer
        // NOTE: might be costly technique performance-wise, beware
        glEnable(GL_MULTISAMPLE);

        glEnable(GL_BLEND);

        float* screenQuadUVCoords = new float[4]{0.0, 1.0, 0.0, 1.0};
        screenQuad = MeshUtils<VertexP1N1UV>::getXYPlane(glm::vec3(0.0f), glm::vec2(1.0f), screenQuadUVCoords);
        delete[] screenQuadUVCoords;

        // Setup framebuffers
        GLenum* shadowBufferAttachments = new GLenum[1]{GL_DEPTH_ATTACHMENT};
        GLenum* mirrorBufferAttachments = new GLenum[1]{GL_COLOR_ATTACHMENT0};
        // TODO: refactor hardcoded shadow map size
        shadowBuffer = new FrameBuffer(1024, 1024, 1, shadowBufferAttachments);
        omniShadowBuffer = new FrameBuffer(1024, 1024, 1, shadowBufferAttachments, true);
        mirrorBuffer = new FrameBuffer(e->getScreenWidth(), e->getScreenHeight(), 1, mirrorBufferAttachments);
        pingpongFBOs[0] = new FrameBuffer(e->getScreenWidth(), e->getScreenHeight(), 1, mirrorBufferAttachments);
        pingpongFBOs[1] = new FrameBuffer(e->getScreenWidth(), e->getScreenHeight(), 1, mirrorBufferAttachments);

        delete[] shadowBufferAttachments;
        delete[] mirrorBufferAttachments;

        pickingTexture = new PickingTexture();
        pickingTexture->init(engine->getScreenWidth(), engine->getScreenHeight());
        pickingShader = Shader::createFromResource("picking");
    }

    RenderingEngine::~RenderingEngine() {
        delete defaultShader;
        delete postFXShader;
        delete normalDebugShader;
        delete dirShadowMapShader;
        delete omnidirShadowMapShader;
        delete skyboxShader;
        delete gaussianBlurShader;
        delete lightColorShader;
        delete pickingShader;

        delete pickingTexture;

        //FIXME: segfault, camera created in application class which user created on stack
        //delete mainCamera;
        delete altCamera;
        delete screenQuad;
        delete shadowBuffer;
        delete omniShadowBuffer;
        delete mirrorBuffer;
    }

    void RenderingEngine::render(SceneNode* node, float deltaTime) {
        if (wireFrameMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Generate picking texture only if mouse is clicked so we can start selecting objects
        if (Input::Get()->isKeyDown(SDL_BUTTON_LEFT)) {
            pickPass(node);
        }
        // 1st Optional Rendering Pass: Render to ambient scene to mirror buffer for rear view mirror effect
        if (mirrorBufferEnabled) {
            mirrorBuffer->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Rotates camera
            glm::vec3 rot = mainCamera->getRotation();
            rot.y += 180.0f;
            mainCamera->setRotation(rot);

            defaultShader->bind();
            defaultShader->setUniformVec3("ambientLight", ambientLight);
            defaultShader->setFogUniforms(*const_cast<RenderingEngine*>(this), *mainCamera);
            activeLight = nullptr;
            node->render(defaultShader, this, mainCamera);
            // Rotate back
            rot.y += 180.0f;
            mainCamera->setRotation(rot);
        }

        // 2nd Rendering Pass: main pass
        //bindMainTarget();
        engine->getSceneBuffer()->bind(); // Rendering to scene buffer for later postfx
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Check if any nodes are selected and use picking texture to get correct node
        if (Input::Get()->isKeyDown(SDL_BUTTON_LEFT)) {
            bool selectableArea = true;
            glm::vec2 clickPosition = Input::Get()->getMouseCoords();

            if (engine->editModeActive()) {
                glm::vec2 sceneImageViewportSize{engine->getImGuiLayer().getSceneViewportWidth(), engine->getImGuiLayer().getSceneViewportHeight()};
                clickPosition = engine->getImGuiLayer().getMousePositionRelativeToScene();

                // Only allow selecting inside sceme viewport ImGui Window
                if (clickPosition.x < 0 ||
                    clickPosition.x > sceneImageViewportSize.x ||
                    clickPosition.y < 0 ||
                    clickPosition.y > sceneImageViewportSize.y) {

                    selectableArea = false;
                }
                // Find real coordinates by finding imgui scene viewport ratio to engine viewport
                clickPosition.x = (engine->getScreenWidth() / sceneImageViewportSize.x) * clickPosition.x;
                clickPosition.y = (engine->getScreenHeight() / sceneImageViewportSize.y) * clickPosition.y;
            }

            if (selectableArea) {
                PickingTexture::PixelInfo pixel = pickingTexture->readPixel(clickPosition.x, Engine::getScreenHeight() - clickPosition.y - 1);
                selectedNodeID = pixel.ObjectID;

                if (pixel.ObjectID != 0) {
                    SceneNode* clickedNode = node->findByID(pixel.ObjectID);
                    if (clickedNode) {
                        engine->getImGuiLayer().getSceneEditor().setSelectedNode(clickedNode);
                    }
                }
            }
        }


        defaultShader->bind();
        defaultShader->setUniformVec3("ambientLight", ambientLight);
        defaultShader->setFogUniforms(*const_cast<RenderingEngine*>(this), *const_cast<Camera*>(mainCamera));
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
            //bindMainTarget();
            engine->getSceneBuffer()->bind();

            //// Using additive blending here to render lights one by one and blend onto the scene
            //// this is so called forward multi-pass rendering
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            light->getShader()->bind();
            light->getShader()->setUniform1i("toonShadingEnabled", toonShadingEnabled);
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

        // after main rendering passes show all the light sources as bright meshes
        std::vector<VertexP1N1UV> vertices;
        std::vector<unsigned int> indices;
        MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, 1.0f);
        Mesh<VertexP1N1UV>* lightSource = new Mesh<VertexP1N1UV>(vertices, indices);
        lightColorShader->bind();
        lightColorShader->setUniformMat4f("projection", mainCamera->getProjMatrix());
        lightColorShader->setUniformMat4f("view", mainCamera->getViewMatrix());

        for (auto& light: lights) {
            lightColorShader->setUniformMat4f("model", light->GetTransform()->getTransformMatrix());
            lightColorShader->setUniformVec3("lightColor", light->DiffuseColor);
            // TODO: really shouldnt need to set material here
            Material mat{"scene", engine->getSceneBuffer()->getTexture(), 1};
            lightSource->draw(*lightColorShader, mat);
        }

        if (visualiseNormals) {
            normalDebugShader->bind();
            node->render(normalDebugShader, this, mainCamera);
        }

        // Always last - render the skybox, if any
        if (currentSkybox) {
            skyboxShader->bind();
            glm::mat4 proj = (mainCamera->getType() == CameraType::ORTHOGRAPHIC) ? mainCamera->getSkyboxProjMatrix() : mainCamera->getProjMatrix();
            currentSkybox->render(proj, mainCamera->getViewMatrix(), deltaTime);
        }
    }

    void RenderingEngine::postRender() {
        // Make sure we disable wireframe mode before post processing pass
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        activeLight = nullptr;

        // Blur bright fragments with 2-pass Gaussian Blur filter
        glDisable(GL_DEPTH_TEST);
        bool horizontal = true, firstIteration = true;
        unsigned int amount = 10;
        gaussianBlurShader->bind();
        for (unsigned int i = 0; i < amount; i++) {
            pingpongFBOs[horizontal]->bind();
            gaussianBlurShader->setUniform1i("horizontal", horizontal);
            Material gaussianBlurMat{"blurredScene", engine->getSceneBuffer()->getTexture(1), 1};
            if (firstIteration) {
                engine->getSceneBuffer()->getTexture(1)->bind();
            } else {
                pingpongFBOs[!horizontal]->getTexture()->bind();
                gaussianBlurMat.setDiffuseMap(pingpongFBOs[!horizontal]->getTexture());
            }
            // Render Quad
            planeTransform.setScale(1.0);
            planeTransform.setPos(glm::vec3(0.0, 0.0, -0.2f));
            gaussianBlurMat.updateUniforms(*gaussianBlurShader);
            gaussianBlurShader->updateUniforms(planeTransform, *this, *altCamera);
            screenQuad->draw(*gaussianBlurShader, gaussianBlurMat);

            horizontal = !horizontal;
            if (firstIteration)
                firstIteration = false;
        }

        bindMainTarget();
        glDisable(GL_DEPTH_TEST);

        engine->getSceneBuffer()->getTexture()->bind();
        pingpongFBOs[!horizontal]->getTexture()->bind(1);
        postFXShader->bind();
        postFXShader->setUniform1i("scene", 0);
        postFXShader->setUniform1i("bloomBlur", 1);
        postFXShader->setUniform1i("invertColors", invertColors);
        postFXShader->setUniform1i("grayScale", grayScale);
        postFXShader->setUniform1i("sharpen", sharpen);
        postFXShader->setUniform1i("blur", blur);
        postFXShader->setUniform1i("edgeDetection", outline);

        postFXShader->setUniform1i("gammaCorrection", gammaCorrection);
        postFXShader->setUniform1i("hdr", hdr);
        postFXShader->setUniform1i("bloom", bloom);
        postFXShader->setUniform1f("exposure", exposure);
        frustumCullingEnabled = false;
        Material postFXMat{"scene", engine->getSceneBuffer()->getTexture(), 1};
        planeTransform.setScale(1.0);
        planeTransform.setPos(glm::vec3(0.0, 0.0, -0.2f));
        planeTransform.setEulerRot(0.0, 180.0, 90.0);
        postFXMat.updateUniforms(*postFXShader);
        postFXShader->updateUniforms(planeTransform, *this, *altCamera);
        screenQuad->draw(*postFXShader, postFXMat);
        defaultShader->bind();

        // Smaller render target on top
        if (mirrorBufferEnabled) {
            Material mirrorMat{"null", mirrorBuffer->getTexture(), 1};
            planeTransform.setScale(0.25);
            planeTransform.setPos(glm::vec3(0.75f, 0.75f, -0.2f));
            mirrorMat.updateUniforms(*defaultShader);
            defaultShader->updateUniforms(planeTransform, *this, *altCamera);
            defaultShader->setUniformVec3("color", ambientLight);
            screenQuad->draw(*defaultShader, mirrorMat);
        }

        frustumCullingEnabled = true;
        glEnable(GL_DEPTH_TEST);
    }

    void RenderingEngine::pickPass(SceneNode* node) {
        // Render all scene nodes to special integer picking texture, used to get correct selected object using mouse
        pickingTexture->enableWriting();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pickingShader->bind();

        activeLight = nullptr;
        node->render(pickingShader, this, mainCamera);

        pickingTexture->disableWriting();
    }

    void RenderingEngine::setSkybox(const std::vector<std::string>& faces) {
        // TODO: handle changing skyboxes, manage memory
        currentSkybox = new SkyBox(faces, skyboxShader);
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

    void RenderingEngine::resize(int newWidth, int newHeight) {
        mainCamera->rescale(newWidth, newHeight);
        pingpongFBOs[0]->rescale(newWidth, newHeight);
        pingpongFBOs[1]->rescale(newWidth, newHeight);
    }
}
