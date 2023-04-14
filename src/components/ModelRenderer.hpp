#ifndef __MODEL_RENDERER__
#define __MODEL_RENDERER__

#include "../rendering/Model.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    class ModelRenderer : public NodeComponent {
        public:
            ModelRenderer(const std::string& fileName);
            ~ModelRenderer();
            virtual void render(
                Shader& shader,
                RenderingEngine& renderingEngine,
                Camera& camera
                ) override;
        private:
            Model* model;
    };
}

#endif // __MODEL_RENDERER__
