#ifndef __FRAMEBUFFER__
#define __FRAMEBUFFER__

#include "Logger.hpp"
#include "Texture.hpp"

namespace Villain {

    class FrameBuffer {
        public:
            FrameBuffer(int w, int h, int textureCount = 1, GLenum* attachments = new GLenum[1]{GL_COLOR_ATTACHMENT0});
            ~FrameBuffer();

            // NOTE: These are not great, in the future might need ability to get multiple textures
            unsigned int getTextureID() const { return textureIDs[0]; }
            Texture* getTexture() const { return textures[0]; }

            void rescale(int w, int h);
            void initTextures();
            void initRenderTargets(GLenum* attachments);

            void bind() const;
            void unbind() const;

        private:
            unsigned int fboID; //<<< framebuffer id
            unsigned int rboID; //<<< renderbuffer id
            int width, height; //<<< framebuffer size
            int numTextures = 1; //<<< number of attached textures
            GLuint* textureIDs; //<<< textures attached to the framebuffer
            Texture** textures;// <<< texture objects used in Rendering Engine
    };
}

#endif // __FRAMEBUFFER__
