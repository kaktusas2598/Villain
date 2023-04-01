#ifndef __RENDERING_ENGINE__
#define __RENDERING_ENGINE__

#include "Camera.hpp"
#include "Light.hpp"
#include "SceneNode.hpp"

namespace Villain {

    // TODO: move all rendering code to here, at least for scene graphs
    class RenderingEngine {
        public:
            RenderingEngine();
            ~RenderingEngine() {}
            void render(SceneNode* node);

            inline void addLight(BaseLight& light) { lights.push_back(&light); }
            inline void setMainCamera(Camera& camera) { mainCamera = &camera; }
            // Call on window/viewport resize event
            void resizeCameras(int newWidth, int newHeight) { mainCamera->rescale(newWidth, newHeight); }
        private:
            // TODO: will need to support multiple cameras at once
            Camera* mainCamera;

            // NOTE: Will need to add way more shaders or do sth different
            // forward shaders for each light type: ambient, directional, point and spot
            Shader* defaultShader;

            std::vector<BaseLight*> lights;
    };
}

#endif // __RENDERING_ENGINE__
