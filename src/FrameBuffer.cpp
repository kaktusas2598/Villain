#include "FrameBuffer.hpp"

namespace Villain {

    FrameBuffer::FrameBuffer(float width, float height) {
        GLCall(glGenFramebuffers(1, &fboID));
        bind();

        GLCall(glGenTextures(1, &textureID));
        GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0));

        GLCall(glGenRenderbuffers(1, &rboID));
        GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rboID));
        GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
        GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID));

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::Instance()->error("Framebuffer not complete.");
        }

        unbind();
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    }

    FrameBuffer::~FrameBuffer() {
        GLCall(glDeleteFramebuffers(1, &fboID));
        GLCall(glDeleteTextures(1, &textureID));
        GLCall(glDeleteRenderbuffers(1, &rboID));
    }

    unsigned int FrameBuffer::getTextureID() {
        return textureID;
    }
    void FrameBuffer::rescale(float width, float height) {
        GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0));

        GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rboID));
        GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
        GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID));
    }


    void FrameBuffer::bind() const {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fboID));
    }

    void FrameBuffer::unbind() const {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

}
