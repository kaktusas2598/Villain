#ifndef __CollisionLayer__
#define __CollisionLayer__

#include "Layer.hpp"
#include <SDL2/SDL.h>
#include <vector>

namespace Villain {

    class CollisionLayer : public Layer {
        public:
            virtual void update();
            virtual void render(SpriteBatch* batch, Camera2D* cam);

            std::vector<SDL_Rect>& getColliders() { return colliders; }
        private:
            std::vector<SDL_Rect> colliders;
            bool collisionRenderingEnabled;
    };
}

#endif // __CollisionLayer__
