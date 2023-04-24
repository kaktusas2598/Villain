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

    // TODO: define sampler map to automatically bind different texture types including
    // shadow maps to their own slot
    class RenderingEngine {
        public:
            RenderingEngine(Engine* e);
            ~RenderingEngine();
            void render(SceneNode* node);
            void postRender();

            inline void addLight(BaseLight* light) { lights.push_back(light); }
            inline BaseLight* getActiveLight() { return activeLight; }
            glm::vec3* getAmbientLightColor() { return &ambientLight; }
            inline glm::mat4 getLightMatrix() const { return lightMatrix; }
            void setMainCamera(Camera& camera) { mainCamera = &camera; }
            inline Camera* getMainCamera() { return mainCamera; }
            // Call on window/viewport resize event
            void resizeCameras(int newWidth, int newHeight) { mainCamera->rescale(newWidth, newHeight); }
        private:
            void bindMainTarget();
            Engine* engine = nullptr;
            Camera* mainCamera = nullptr;
            Camera* altCamera = nullptr;

            Mesh<VertexP1N1UV>* screenQuad = nullptr;
            Transform planeTransform;
            FrameBuffer* shadowBuffer = nullptr;
            FrameBuffer* mirrorBuffer = nullptr;

            Shader* defaultShader = nullptr;
            Shader* shadowMapShader = nullptr;

            BaseLight* activeLight = nullptr;
            std::vector<BaseLight*> lights;
            glm::vec3 ambientLight = glm::vec3(0.5f);
            glm::mat4 lightMatrix = glm::mat4(1.0f);
    };
}

#endif // __RENDERING_ENGINE__
