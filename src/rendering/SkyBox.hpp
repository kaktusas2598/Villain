#ifndef __SKYBOX__
#define __SKYBOX__

#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"

#include <memory>

namespace Villain {

    class SkyBox {
        public:
            // Cubemap face texture paths must be in this order: Right, Left, Top, Bottom, Front, Back!
            SkyBox(const std::vector<std::string>& faces, const std::string& cubemapShaderPath);
            SkyBox(const std::vector<std::string>& faces, Shader* shader);
            ~SkyBox() {}

            // Must be rendered LAST every frame
            void render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
        private:
            std::unique_ptr<VertexArray> skyboxVao;
            std::unique_ptr<VertexBuffer> skyboxVbo;
            std::unique_ptr<IndexBuffer> skyboxIbo;

            void setupBuffers();

            Shader* cubemapShader;
            Texture* cubemapTexture;

            // TODO: Environmental mapping support for reflections and refractions
            bool reflect = false;
            bool refract = false;
    };
}

#endif // __SKYBOX__
