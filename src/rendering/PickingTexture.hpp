#ifndef __PICKING_TEXTURE__
#define __PICKING_TEXTURE__

#include "ErrorHandler.hpp"

namespace Villain {

    class PickingTexture {
        public:
            PickingTexture() {}
            ~PickingTexture();

            void init(unsigned int windowWidth, unsigned int windowHeight);

            void enableWriting();
            void disableWriting();

            struct PixelInfo {
                unsigned int ObjectID;
                unsigned int DrawID;
                unsigned int PrimID;
            };

            PixelInfo readPixel(unsigned int x, unsigned int y);

        private:
            GLuint framebufferID;
            GLuint pickingTextureID;
            GLuint depthTextureID;
    };
};

#endif // __PICKING_TEXTURE__DEBUG
