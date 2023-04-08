#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "Camera3D.hpp"
#include "Level.hpp"

class Game : public Villain::Application {
    public:
        ~Game();

        void init();
        void addStates() {}
        void onAppPreUpdate(float dt);

    private:
        Villain::Camera3D camera;
        Level* currentLevel;
};

#endif // __GAME__
