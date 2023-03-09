#ifndef __GAME__
#define __GAME__

#include "ResourceManager.hpp"
#include "Sprite.hpp"
#include "SpriteBatch.hpp"
#include "Timer.hpp"

#include "Model.hpp"
#include "Camera.hpp"

class Game {
    public:
        Game();
        ~Game();

        static void handleEvents();
        static void run();
        // Villain Engine callbacks
        static void preUpdate(float dt);
        static void postUpdate(float dt);
        static void preRender(float dt);
        static void postRender(float dt);
        static void onWindowResize(int newWidth, int newHeight);

    private:
        //NOTE: In example game code, most of Game class members need to be static
        //so they can be used in engine callbacks, not sure if this is good idea, but it works well
        //and this way engine can be used to create all kinds of applications possibly
        static Camera camera;
        static Model* model;




};

#endif // __GAME__
