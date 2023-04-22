#ifndef __RENDERING_ENGINE__
#define __RENDERING_ENGINE__

#include "Camera.hpp"
#include "../components/Light.hpp"
#include "SceneNode.hpp"

namespace Villain {

    class RenderingEngine {
        public:
            RenderingEngine(Engine* e);
            ~RenderingEngine();
            void render(SceneNode* node);

            inline void addLight(BaseLight* light) { lights.push_back(light); }
            inline BaseLight* getActiveLight() { return activeLight; }
            glm::vec3* getAmbientLightColor() { return &ambientLight; }
            void setMainCamera(Camera& camera) { mainCamera = &camera; }
            inline Camera* getMainCamera() { return mainCamera; }
            // Call on window/viewport resize event
            void resizeCameras(int newWidth, int newHeight) { mainCamera->rescale(newWidth, newHeight); }
        private:
            Engine* engine = nullptr;
            // TODO: will need to support multiple cameras at once
            Camera* mainCamera;

            Shader* defaultShader = nullptr;

            BaseLight* activeLight = nullptr;
            std::vector<BaseLight*> lights;
            glm::vec3 ambientLight = glm::vec3(0.2f);
    };
}

#endif // __RENDERING_ENGINE__
