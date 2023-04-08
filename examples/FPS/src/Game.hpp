#ifndef __GAME__
#define __GAME__

#include "Application.hpp"

#include "Camera3D.hpp"

class Game : public Villain::Application {
    public:
        ~Game();

        void init();
        void addStates() {}
        void onAppPreUpdate(float dt);

    private:
        Villain::Camera3D camera;
};

#endif // __GAME__
