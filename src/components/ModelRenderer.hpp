#ifndef __MODEL_RENDERER__
#define __MODEL_RENDERER__

#include "NodeComponent.hpp"
#include "rendering/animation/Animator.hpp"
#include "rendering/Model.hpp"

namespace Villain {

    class ModelRenderer : public NodeComponent {
        public:
            ModelRenderer(Model *m, const std::string& animationPath = std::string()) : model(m) {
                if (!animationPath.empty()) {
                    currentAnimation = new Animation(animationPath, model);
                    animator = new Animator(currentAnimation);
                }
            };
            ~ModelRenderer();

            virtual void render(
                Shader& shader,
                RenderingEngine& renderingEngine,
                Camera& camera
                ) override;
            virtual void update(float deltaTime) override;

            Model* getModel() const { return model; }
            Animation* getCurrentAnimation() const { return currentAnimation; }
            Animator* getAnimator() const { return animator; }
        private:
            Model* model;

            // Optional skeletal animation properties
            Animation* currentAnimation = nullptr;
            Animator* animator = nullptr;
    };
}

#endif // __MODEL_RENDERER__
