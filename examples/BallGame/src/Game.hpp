#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "ResourceManager.hpp"
#include "SpriteBatch.hpp"
#include "Camera2D.hpp"
#include "Timer.hpp"

#include "FreeType.hpp"
#include <memory>
#include <vector>

#include "Ball.hpp"
#include "Grid.hpp"
#include "BallController.hpp"
#include "BallRenderer.hpp"

enum class GameState { RUNNING, EXIT };

const int CELL_SIZE = 12;

class Game : public Villain::Application {
    public:
        ~Game();

        void init();

        void handleEvents();
        void initBalls();

        void onInit() {};
        void addStates() {};
        void onAppPreUpdate(float deltaTime);
        void onAppPostUpdate(float deltaTime);
        void onAppRender(float deltaTime);
        void onAppWindowResize(int newWidth, int newHeight);

        void onMouseMove(int mouseX, int mouseY);
        void onMouseDown(int mouseX, int mouseY);
        void onMouseUp();

    private:
        BallController ballController;
        std::vector<Ball> balls;
        std::unique_ptr<Grid> grid; //<<< Spatial partitioning grid for collision

        int currentRenderer = 0;
        std::vector<std::unique_ptr<BallRenderer>> ballRenderers;


        Villain::SpriteBatch spriteBatch;
        Villain::Camera2D camera;
        Villain::Camera2D hudCamera;

        Villain::Timer colorTimer;

        Villain::SpriteBatch textBatch;
        Villain::FreeType* freeType;

        GameState currentState = GameState::RUNNING;
};

#endif // __GAME__
