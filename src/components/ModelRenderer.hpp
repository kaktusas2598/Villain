#ifndef __MODEL_RENDERER__
#define __MODEL_RENDERER__

#include "../rendering/Model.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    class ModelRenderer : public NodeComponent {
        public:
            ModelRenderer(Model *m) : model(m) {};
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
