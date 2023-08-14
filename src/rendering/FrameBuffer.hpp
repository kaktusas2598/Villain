#pragma once

#include "Logger.hpp"
#include "Texture.hpp"

namespace Villain {

    class FrameBuffer {
        public:
            // @param cubeMap - if true, generate and attach cubemap directly to a framebuffer, else use GL_TEXTURE_2D as attachments
            FrameBuffer(int w, int h, int textureCount = 1, GLenum* attachments = new GLenum[1]{GL_COLOR_ATTACHMENT0}, bool cubeMap = false);
            ~FrameBuffer();

            unsigned int getTextureID(unsigned textureIndex = 0) const;
            Texture* getTexture(unsigned textureIndex = 0) const;
            int getWidth() const { return width; }
            int getHeight() const { return height; }

            void rescale(int w, int h);
            void initTextures(GLenum* attachments, GLenum target);
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
