#pragma once

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
            Animator* getAnimator() { return model->getAnimator(); }
            Animation* getCurrentAnimation() { return getAnimator() ? getAnimator()->getCurrentAnimation() : nullptr; }
        private:
            Model* model;
    };
}
