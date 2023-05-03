#ifndef __RENDERING_ENGINE__
#define __RENDERING_ENGINE__

#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "../components/Light.hpp"
#include "SceneNode.hpp"
#include "Vertex.hpp"

namespace Villain {

    template <class VertexType>
    class Mesh;
    class Texture;

    class RenderingEngine {
        public:
            RenderingEngine(Engine* e);
            ~RenderingEngine();
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
            // Call on window/viewport resize event
            void resizeCameras(int newWidth, int newHeight) { mainCamera->rescale(newWidth, newHeight); }

            // Post-processing effects/filters
            bool* getInvertColors() { return &invertColors; }
            bool* getGrayScale() { return &grayScale; }
            bool* getSharpen() { return &sharpen; }
            bool* getBlur() { return &blur; }
            bool* getEdgeDetection() { return &outline; }

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

            Shader* defaultShader = nullptr;
            Shader* postFXShader = nullptr;
            Shader* dirShadowMapShader = nullptr;
            Shader* omnidirShadowMapShader = nullptr;

            BaseLight* activeLight = nullptr;
            std::vector<BaseLight*> lights;
            static std::map<std::string, unsigned int> samplerMap;
            glm::vec3 ambientLight = glm::vec3(0.5f);
            glm::mat4 lightMatrix = glm::mat4(1.0f);

            bool invertColors = false;
            bool grayScale = false;
            bool sharpen = false;
            bool blur = false;
            bool outline = false;
    };
}

#endif // __RENDERING_ENGINE__
