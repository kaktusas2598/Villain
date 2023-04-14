#ifndef __FRAMEBUFFER__
#define __FRAMEBUFFER__

#include "Logger.hpp"

namespace Villain {

    class FrameBuffer {
        public:
            FrameBuffer(float width, float height);
            ~FrameBuffer();

            unsigned int getTextureID();
            void rescale(float width, float height);

            void bind() const;
            void unbind() const;

        private:
            unsigned int fboID; //<<< framebuffer id
            unsigned int textureID; //<<< fbo will render to this texture
            unsigned int rboID; //<<< renderbuffer id
    };
}

#endif // __FRAMEBUFFER__
