#ifndef __MODEL_RENDERER__
#define __MODEL_RENDERER__

#include "NodeComponent.hpp"
#include "rendering/animation/Animator.hpp"
#include "rendering/Model.hpp"

namespace Villain {

    class ModelRenderer : public NodeComponent {
        public:
            ModelRenderer(Model *m, float animationSpeed = 0.0f) : model(m) {
                if (m->getAnimator()) {
                    if (animationSpeed > 0) m->getAnimator()->getCurrentAnimation()->setSpeed(animationSpeed);
                }
            };
            ~ModelRenderer() {}

            virtual void render(
                Shader& shader,
                RenderingEngine& renderingEngine,
                Camera& camera
                ) override;
            virtual void update(float deltaTime) override;

            Model* getModel() const { return model; }
            Animator* getAnimator() const { return model->getAnimator(); }
            Animation* getCurrentAnimation() const { return model->getAnimator()->getCurrentAnimation(); }
        private:
            Model* model;
    };
}

#endif // __MODEL_RENDERER__
