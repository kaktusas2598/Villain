#ifndef __BALL_RENDERER__
#define __BALL_RENDERER__

#include "Shader.hpp"
#include "SpriteBatch.hpp"
#include "Ball.hpp"

class BallRenderer {
    public:
        virtual void renderBalls(Villain::SpriteBatch& batch, const std::vector<Ball>& balls, const glm::mat4& viewMatrix);
    protected:
        Villain::Shader* shader = nullptr;

};

class MomentumBallRenderer: public BallRenderer {
    public:
        void renderBalls(Villain::SpriteBatch& batch, const std::vector<Ball>& balls, const glm::mat4& viewMatrix) override;
};

class VelocityBallRenderer: public BallRenderer {
    public:
        void renderBalls(Villain::SpriteBatch& batch, const std::vector<Ball>& balls, const glm::mat4& viewMatrix) override;
};

class TrippyBallRenderer: public BallRenderer {
    public:
        void renderBalls(Villain::SpriteBatch& batch, const std::vector<Ball>& balls, const glm::mat4& viewMatrix) override;
    private:
        float time = 0.0f;
};


#endif // __BALL_RENDERER__
