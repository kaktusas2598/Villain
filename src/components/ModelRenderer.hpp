#ifndef __MODEL_RENDERER__
#define __MODEL_RENDERER__

#include "../rendering/Model.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    class ModelRenderer : public NodeComponent {
        public:
            ModelRenderer(const std::string& fileName, unsigned instances = 1, std::vector<glm::mat4> instanceTransforms={});
            ~ModelRenderer();
            virtual void render(
                Shader& shader,
                RenderingEngine& renderingEngine,
                Camera& camera
                ) override;

            Model* getModel() const { return model; }
        private:
            Model* model;
    };
}

#endif // __MODEL_RENDERER__
