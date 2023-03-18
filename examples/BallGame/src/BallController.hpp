#ifndef __BALL_CONTROLLER__
#define __BALL_CONTROLLER__

#include <vector>
#include "Ball.hpp"

enum class GravityDirection {NONE, LEFT, UP, RIGHT, DOWN};

class Grid;

class BallController {
    public:
        void updateBalls(std::vector<Ball>& balls, Grid* grid, float deltaTime, int maxX, int maxY);

        void onMouseDown(std::vector<Ball>& balls, float mouseX, float mouseY);
        void onMouseUp(std::vector<Ball>& balls);
        void onMouseMove(std::vector<Ball>& balls, float mouseX, float mouseY);

        void setGravityDirection(GravityDirection dir) { gravityDirection = dir; }

    private:
        void updateCollision(Grid* grid);
        void checkCollision(Ball* ball, std::vector<Ball*>& ballsToCheck, int startingIndex);
        void checkCollision(Ball& b1, Ball& b2);
        bool isMouseOnBall(Ball&b, float mouseX, float mouseY);
        glm::vec2 getGravityAcc();

        int grabbedBall = -1; //<<< Currently grabbed ball
        glm::vec2 prevPos = glm::vec2(0.0f); //<<< Previous position of a grabbed ball
        glm::vec2 grabOffset = glm::vec2(0.0f); //<<< Offset of the cursor on the selected ball

        GravityDirection gravityDirection = GravityDirection::NONE;
};

#endif // __BALL_CONTROLLER__
