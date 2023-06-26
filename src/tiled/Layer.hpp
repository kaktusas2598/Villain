#ifndef __LAYER__
#define __LAYER__

#include "Camera.hpp"
#include "rendering/SpriteBatch.hpp"

namespace Villain {

    // 2D Tiled Map single Layer which can hold Tile, Entity/Agent or Collider information
    class Layer {
        public:
            virtual void init() {};
            virtual void update() = 0;
            virtual void render(SpriteBatch* batch, Camera* cam) = 0;
        //protected:
            virtual ~Layer() {}
    };
}

#endif // __LAYER__
