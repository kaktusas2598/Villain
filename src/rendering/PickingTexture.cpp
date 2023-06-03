#include "PickingTexture.hpp"

#include "Logger.hpp"

namespace Villain {

    PickingTexture::~PickingTexture() {}

    // TODO: refactor to use Framebuffer class
    void PickingTexture::init(unsigned int windowWidth, unsigned int windowHeight) {
        GLCall(glGenFramebuffers(1, &framebufferID));
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, framebufferID));

        // Create texture handle for primitive information buffer
        GLCall(glGenTextures(1, &pickingTextureID));
        GLCall(glBindTexture(GL_TEXTURE_2D, pickingTextureID));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, windowWidth, windowHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTextureID, 0));

        // Create texture handle for depth buffer
        GLCall(glGenTextures(1, &depthTextureID));
        GLCall(glBindTexture(GL_TEXTURE_2D, depthTextureID));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureID, 0));

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::Instance()->error("Framebuffer not complete.");
        }

        // Back to default framebuffer
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void PickingTexture::enableWriting() {
        GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID));
    }

    void PickingTexture::disableWriting() {
        GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    }

    PickingTexture::PixelInfo PickingTexture::readPixel(unsigned int x, unsigned int y) {
        GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID));
        GLCall(glReadBuffer(GL_COLOR_ATTACHMENT0));

        PixelInfo pixel;
        GLCall(glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel));

        GLCall(glReadBuffer(GL_NONE));
        GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));

        return pixel;
    }
}
