#ifndef __RENDERING_ENGINE__
#define __RENDERING_ENGINE__

#include "Camera.hpp"
#include "Light.hpp"
#include "SceneNode.hpp"

namespace Villain {

    class RenderingEngine {
        public:
            RenderingEngine();
            ~RenderingEngine();
            void render(SceneNode* node);

            inline void addLight(BaseLight* light) { lights.push_back(light); }
            inline BaseLight* getActiveLight() { return activeLight; }
            void setMainCamera(Camera& camera) { mainCamera = &camera; }
            inline Camera* getMainCamera() { return mainCamera; }
            // Call on window/viewport resize event
            void resizeCameras(int newWidth, int newHeight) { mainCamera->rescale(newWidth, newHeight); }
        private:
            // TODO: will need to support multiple cameras at once
            Camera* mainCamera;

            Shader* defaultShader = nullptr;

            BaseLight* activeLight = nullptr;
            std::vector<BaseLight*> lights;
    };
}

#endif // __RENDERING_ENGINE__
