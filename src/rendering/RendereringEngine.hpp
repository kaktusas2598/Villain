#ifndef __RENDERING_ENGINE__
#define __RENDERING_ENGINE__

#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "../components/Light.hpp"
#include "SceneNode.hpp"
#include "Vertex.hpp"
#include "rendering/PickingTexture.hpp"

namespace Villain {

    template <class VertexType>
    class Mesh;
    class Texture;

    class RenderingEngine {
        public:
            RenderingEngine(Engine* e);
            ~RenderingEngine();
            void pickPass(SceneNode* node);
            void render(SceneNode* node);
            void postRender();

            inline void addLight(BaseLight* light) { lights.push_back(light); }
            inline BaseLight* getActiveLight() { return activeLight; }
            static unsigned int getSamplerSlot(const std::string& samplerType) { return samplerMap.find(samplerType)->second; }
            glm::vec3* getAmbientLightColor() { return &ambientLight; }
            inline glm::mat4 getLightMatrix() const { return lightMatrix; }
            void setMainCamera(Camera& camera) { mainCamera = &camera; }
            inline Camera* getMainCamera() { return mainCamera; }
            inline bool isFrustumCullingEnabled() { return frustumCullingEnabled; }
            bool* getGammaCorrection() { return &gammaCorrection; }
            void setGammaCorrection(bool gamma) { gammaCorrection = gamma; }
            static bool gammaCorrectionEnabled() { return gammaCorrection; }
            int getSelectedNodeID() const { return selectedNodeID; }

            // Fog parameters
            glm::vec3* getFogColor() { return &fogColor; }
            float* getFogDensity() { return &fogDensity; }
            float* getFogGradient() { return &fogGradient; }
            float* getLayeredFogTop() { return &layeredFogTop; }
            float* getLayeredFogEnd() { return &layeredFogEnd; }
            bool* exponentialFogEnabled() { return &useExponentialFog; }

            // Call on window/viewport resize event
            void resizeCameras(int newWidth, int newHeight) { mainCamera->rescale(newWidth, newHeight); }

            // Post-processing effects/filters
            bool* getInvertColors() { return &invertColors; }
            bool* getGrayScale() { return &grayScale; }
            bool* getSharpen() { return &sharpen; }
            bool* getBlur() { return &blur; }
            bool* getEdgeDetection() { return &outline; }
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

            PickingTexture* pickingTexture = nullptr;
            Shader* pickingShader = nullptr;

            Shader* defaultShader = nullptr;
            Shader* postFXShader = nullptr;
            Shader* dirShadowMapShader = nullptr;
            Shader* omnidirShadowMapShader = nullptr;

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

            static bool gammaCorrection;

            int selectedNodeID = 0;
    };
}

#endif // __RENDERING_ENGINE__
