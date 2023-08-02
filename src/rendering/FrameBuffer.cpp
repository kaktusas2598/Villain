#include "FrameBuffer.hpp"

namespace Villain {

    FrameBuffer::FrameBuffer(int w, int h, int textureCount, GLenum* attachments, bool cubeMap) :
        fboID(0), rboID(0), width(w), height(h), numTextures(textureCount)
    {
        textureIDs = new GLuint[textureCount];
        textures = new Texture*[textureCount];
        if (cubeMap) {
            initTextures(attachments, GL_TEXTURE_CUBE_MAP);
        } else {
            initTextures(attachments, GL_TEXTURE_2D);
        }
        initRenderTargets(attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::Instance()->error("Framebuffer not complete.");
        }
    }

    void FrameBuffer::initTextures(GLenum* attachments, GLenum target) {
        GLCall(glGenTextures(numTextures, textureIDs));
        for (int i = 0; i < numTextures; i++) {
            textures[i] = new Texture(target);
            if (attachments[i] == GL_DEPTH_ATTACHMENT) {
                if (target == GL_TEXTURE_2D)
                    textures[i]->init(width, height, textureIDs[i], GL_NEAREST, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, true);
                else
                    textures[i]->initCubeMap(width, height, textureIDs[i], GL_NEAREST, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
            } else {
                if (target == GL_TEXTURE_2D)
                    // NOTE: need to investigate if SRGB internal format is required for Framebuffer texture attachments when
                    // gamma correcting in post-processing quad
                    textures[i]->init(width, height, textureIDs[i], GL_NEAREST, GL_RGBA, GL_RGBA, false);
                else
                    textures[i]->initCubeMap(width, height, textureIDs[i], GL_NEAREST, GL_RGBA, GL_RGBA);
            }
        }
    }

    void FrameBuffer::initRenderTargets(GLenum* attachments) {
        if (attachments == nullptr) {
            return;
        }

        GLenum drawBuffers[numTextures];
        bool hasDepth = false;

        for (int i = 0; i < numTextures; i++) {
            // NOTE: Need another condition for GL_STENCIL_ATTACHMENT
            if (attachments[i] == GL_DEPTH_ATTACHMENT) {
                drawBuffers[i] = GL_NONE;
                hasDepth = true;
            } else {
                drawBuffers[i] = attachments[i];
            }

            if (attachments[i] == GL_NONE) {
                continue;
            }

            if (fboID == 0) {
                GLCall(glGenFramebuffers(1, &fboID));
                GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fboID));
            }

            GLCall(glFramebufferTexture(GL_FRAMEBUFFER, attachments[i], textureIDs[i], 0));
        }

        if (fboID == 0) {
            return;
        }

        if (!hasDepth) {
            GLCall(glGenRenderbuffers(1, &rboID));
            GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rboID));
            GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
            GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID));
        }

        //glDrawBuffer(GL_NONE);
        //glReadBuffer(GL_NONE);

        // Seems to work fine without this?
        GLCall(glDrawBuffers(numTextures, drawBuffers));
    }

    FrameBuffer::~FrameBuffer() {
        if (*textureIDs) GLCall(glDeleteTextures(numTextures, textureIDs));
        if (fboID) GLCall(glDeleteFramebuffers(1, &fboID));
        if (rboID) GLCall(glDeleteRenderbuffers(1, &rboID));
        if (textureIDs) delete[] textureIDs;
        // FIXME: segfault
        //for (int i = 0; i < numTextures; ++i)
            //delete textures[i];
        delete[] textures;
    }

    void FrameBuffer::rescale(int w, int h) {
        width = w;
        height = h;
        bind();
        // FIXME: had to remove couple of GLCall(), need to try recreating
        // fbo every time we resize viewport probably as some functions below were causing errors
        for (int i = 0; i < numTextures; i++) {
            textures[i]->init(w, h, textureIDs[i]);
            // TODO: this is bad design once we start attaching depth and stencil attachments
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureIDs[i], 0);

        }
        if (rboID != 0) {
            GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rboID));
            GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h));
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);
        }
    }

    void FrameBuffer::bind() const {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fboID)); // or GL_DRAW_FRAMEBUFFER as target??
        GLCall(glViewport(0, 0, width, height)); // Good idea everytime binding to new render target to set size accordingly
    }

    void FrameBuffer::unbind() const {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}
