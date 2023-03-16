#ifndef __BALL_RENDERER__
#define __BALL_RENDERER__

#include "SpriteBatch.hpp"
#include "Ball.hpp"

class BallRenderer {
    public:
        void renderBall(Villain::SpriteBatch& batch, Ball& ball);
};

#endif // __BALL_RENDERER__
