#include "HDRMap.hpp"

#include "Engine.hpp"
#include "Renderer.hpp"

namespace Villain {

    HDRMap::HDRMap(const std::string& hdrTexturePath) {
        setupBuffers();

        // Make sure OpenGL filters between cubemap faces to avoid visible seams in prefiltered convolution maps
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        equirectangularToCubeMap = Shader::createFromResource("equirectangularToCubemap");
        cubemapShader = Shader::createFromResource("envmap");
        irradianceConvolutionShader = Shader::createFromResource("irradianceConvolution");
        prefilterConvolutionShader = Shader::createFromResource("preFilterConvolution");

        // Load HDR environment map image file to texture
        hdrTexture = new Texture(GL_TEXTURE_2D);
        hdrTexture->loadHDR(hdrTexturePath);

        // Setup fbo to capture env map from 6 sides to build cubemap
        unsigned int captureFBO, captureRBO;
        GLCall(glGenFramebuffers(1, &captureFBO));
        GLCall(glGenRenderbuffers(1, &captureRBO));

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
        GLCall(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
        GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048));
        GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO));
        ///////////////////////////////////////////////////

        // Setup cubemap to render to and attach to framebuffer
        GLCall(glGenTextures(1, &envCubemap));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap));
        for (unsigned int i = 0; i < 6; ++i) {
            // note that we store each face with 16 bit floating point values for capturing hdr map to cubemap
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                    2048, 2048, 0, GL_RGB, GL_FLOAT, nullptr));
        }
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        ///////////////////////////////////////////////////

        // Setup projection matrix and view matrices to capture data onto 6 cubemap faces
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };
        ///////////////////////////////////////////////////

        // convert HDR equirectangular environment map to cubemap equivalent
        equirectangularToCubeMap->bind();
        equirectangularToCubeMap->setUniform1i("equirectangularMap", 0);
        equirectangularToCubeMap->setUniformMat4f("projection", captureProjection);
        hdrTexture->bind();

        glDisable(GL_CULL_FACE);
        GLCall(glViewport(0, 0, 2048, 2048)); // don't forget to configure the viewport to the capture dimensions.
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
        Renderer renderer;
        for (unsigned int i = 0; i < 6; ++i) {
            equirectangularToCubeMap->setUniformMat4f("view", captureViews[i]);
            GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            renderer.draw(*skyboxVao, *skyboxIbo, *equirectangularToCubeMap);
        }

        // then generate mipmaps to combat visible dots artifact
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap));
        GLCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
        ///////////////////////////////////////////////////

        // Setup irradiance map texture
        GLCall(glGenTextures(1, &irradianceMap));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap));
        for (unsigned int i = 0; i < 6; ++i)
        {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0,
                    GL_RGB, GL_FLOAT, nullptr));
        }
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        ///////////////////////////////////////////////////

        // Rescale capture FBO to new resolution for convoluted map
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
        GLCall(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
        GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 128, 128));

        // Solve diffuse integral by convolution to create irradiance map
        irradianceConvolutionShader->bind();
        irradianceConvolutionShader->setUniform1i("environmentMap", 0);
        irradianceConvolutionShader->setUniformMat4f("projection", captureProjection);
        GLCall(glActiveTexture(GL_TEXTURE0));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap));

        GLCall(glViewport(0, 0, 128, 128)); // don't forget to configure the viewport to the capture dimensions.
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
        for (unsigned int i = 0; i < 6; ++i) {
            irradianceConvolutionShader->setUniformMat4f("view", captureViews[i]);
            GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            renderer.draw(*skyboxVao, *skyboxIbo, *irradianceConvolutionShader);
        }
        ///////////////////////////////////////////////////

        // Setup prefiltered env map texture for specular IBL
        GLCall(glGenTextures(1, &prefilteredMap));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMap));
        for (unsigned int i = 0; i < 6; ++i) {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0,
                    GL_RGB, GL_FLOAT, nullptr));
        }
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GLCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
        ///////////////////////////////////////////////////

        // Capture Pre-filter mipmap levels
        prefilterConvolutionShader->bind();
        prefilterConvolutionShader->setUniform1i("environmentMap", 0);
        prefilterConvolutionShader->setUniformMat4f("projection", captureProjection);
        GLCall(glActiveTexture(GL_TEXTURE0));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap));

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            // Resize fbo in regards to mipmap size
            unsigned int mipWidth = 128 * std::pow(0.5, mip);
            unsigned int mipHeight = 128 * std::pow(0.5, mip);
            GLCall(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
            GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight));
            GLCall(glViewport(0, 0, mipWidth, mipHeight));

            float roughness = (float)mip / (maxMipLevels - 1);
            prefilterConvolutionShader->setUniform1f("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i) {
                prefilterConvolutionShader->setUniformMat4f("view", captureViews[i]);
                GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredMap, mip));
                GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

                renderer.draw(*skyboxVao, *skyboxIbo, *prefilterConvolutionShader);
            }
        }
        ///////////////////////////////////////////////////


        glEnable(GL_CULL_FACE);
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glViewport(0, 0, Engine::getScreenWidth(), Engine::getScreenHeight()));
    }

    void HDRMap::render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float exposure) {
        Renderer renderer;

        GLCall(glDepthFunc(GL_EQUAL));
        cubemapShader->bind();
        cubemapShader->setUniformMat4f("projection", projectionMatrix);
        cubemapShader->setUniformMat4f("view", viewMatrix);
        cubemapShader->setUniform1f("exposure", exposure);

        GLCall(glActiveTexture(GL_TEXTURE0));
        //GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap));
        //GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMap));
        cubemapShader->setUniform1i("environmentMap", 0);
        renderer.draw(*skyboxVao, *skyboxIbo, *cubemapShader);
        GLCall(glDepthFunc(GL_LESS)); // Back to default
    }
}
