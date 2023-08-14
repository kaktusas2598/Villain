#pragma once

#include "camera/Camera.hpp"
#include "FrameBuffer.hpp"
#include "../components/Light.hpp"
#include "SceneNode.hpp"
#include "Vertex.hpp"
#include "rendering/PickingTexture.hpp"
#include "rendering/SkyBox.hpp"

namespace Villain {

    template <class VertexType>
    class Mesh;
    class Texture;

    class RenderingEngine {
        public:
            RenderingEngine(Engine* e);
            ~RenderingEngine();
            void pickPass(SceneNode* node);
            void render(SceneNode* node, float deltaTime);
            void postRender();

            inline void addLight(BaseLight* light) { lights.push_back(light); }
            void removeLight(BaseLight* light) {
                lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
            }
            inline BaseLight* getActiveLight() { return activeLight; }
            static unsigned int getSamplerSlot(const std::string& samplerType) { return samplerMap.find(samplerType)->second; }
            glm::vec3* getAmbientLightColor() { return &ambientLight; }
            void setAmbientLightColor(const glm::vec3& color) { ambientLight = color; }
            inline glm::mat4 getLightMatrix() const { return lightMatrix; }
            void setMainCamera(Camera& camera) { mainCamera = &camera; }
            inline Camera* getMainCamera() { return mainCamera; }
            inline bool isFrustumCullingEnabled() { return frustumCullingEnabled; }
            bool* getGammaCorrection() { return &gammaCorrection; }
            bool* getHDR() { return &hdr; }
            bool* getBloom() { return &bloom; }
            float* getExposure() { return &exposure; }
            void setGammaCorrection(bool gamma) { gammaCorrection = gamma; }
            static bool gammaCorrectionEnabled() { return gammaCorrection; }
            int getSelectedNodeID() const { return selectedNodeID; }
            void setSelectedNodeID(int id) { selectedNodeID = id; }
            void setSkybox(const std::vector<std::string>& faces);

            // Fog parameters
            glm::vec3* getFogColor() { return &fogColor; }
            float* getFogDensity() { return &fogDensity; }
            float* getFogGradient() { return &fogGradient; }
            float* getLayeredFogTop() { return &layeredFogTop; }
            float* getLayeredFogEnd() { return &layeredFogEnd; }
            bool* exponentialFogEnabled() { return &useExponentialFog; }

            // Call on window/viewport resize event
            void resize(int newWidth, int newHeight);

            // Post-processing effects/filters
            bool* getInvertColors() { return &invertColors; }
            bool* getGrayScale() { return &grayScale; }
            bool* getSharpen() { return &sharpen; }
            bool* getBlur() { return &blur; }
            bool* getEdgeDetection() { return &outline; }
            bool* getToonShadingEnabled() { return &toonShadingEnabled; }
            bool* getVisualiseNormals() { return &visualiseNormals; }
            bool* getVisualiseBoneWeights() { return &visualiseBoneWeights; }
            bool* wireFrameModeActive() { return &wireFrameMode; }
            // Additional FBOs
            bool* getMirrorFramebufferEnabled() { return &mirrorBufferEnabled; }

        private:
            void bindMainTarget();
            Engine* engine = nullptr;
            Camera* mainCamera = nullptr;
            Camera* altCamera = nullptr;
            bool frustumCullingEnabled = true;

            Mesh<VertexP1N1UV>* screenQuad = nullptr;
            Transform planeTransform;
            FrameBuffer* shadowBuffer = nullptr;
            FrameBuffer* omniShadowBuffer = nullptr;
            FrameBuffer* mirrorBuffer = nullptr;
            FrameBuffer* pingpongFBOs[2]; ///< For blurring scene used for bloom/glow effect

            // Mouse picking attributes
            PickingTexture* pickingTexture = nullptr;
            Shader* pickingShader = nullptr;

            // Optional Skybox attrivutes
            SkyBox* currentSkybox = nullptr;
            Shader* skyboxShader = nullptr;

            Shader* defaultShader = nullptr;
            Shader* postFXShader = nullptr;
            Shader* normalDebugShader = nullptr;
            Shader* dirShadowMapShader = nullptr;
            Shader* omnidirShadowMapShader = nullptr;
            Shader* gaussianBlurShader = nullptr;
            // Maybe temporary shader to render light sources themselves as single color
            Shader* lightColorShader = nullptr;

            BaseLight* activeLight = nullptr;
            std::vector<BaseLight*> lights;
            static std::map<std::string, unsigned int> samplerMap;
            glm::vec3 ambientLight = glm::vec3(0.5f);
            glm::mat4 lightMatrix = glm::mat4(1.0f);

            glm::vec3 fogColor = glm::vec3(0.0f); //<<< Common for all fog types
            bool useExponentialFog = false;
            // Exponential Fog Parameters
            float fogDensity = 0.007f;
            float fogGradient = 1.5f;
            // Layered Fog Parameters
            float layeredFogTop = 250.0f; // Maximum height of fog
            float layeredFogEnd = 100.0f; // Max distance?

            bool invertColors = false;
            bool grayScale = false;
            bool sharpen = false;
            bool blur = false;
            bool outline = false;
            bool mirrorBufferEnabled = false;
            bool toonShadingEnabled = false;
            bool visualiseNormals = false;
            bool visualiseBoneWeights = false;
            bool wireFrameMode = false; ///< Optional wireframe rendering mode

            static bool gammaCorrection;
            bool hdr = false;
            bool bloom = false;
            float exposure = 1.0; ///< Used to gether with HDR

            int selectedNodeID = 0;
    };
}
