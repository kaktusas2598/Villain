#pragma once

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
            /// Default constructor for derived classes like HDRMap
            SkyBox() {}
            virtual ~SkyBox() {}

            // Must be rendered LAST every frame
            void render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float deltaTime);

            void setAnimated(bool anim = false) { animated = anim; }
            void setRotationSpeed(float speed) { rotationSpeed = speed; }

            bool* getAnimated() { return &animated; }
            float* getRotationSpeed() { return &rotationSpeed; }

        protected:
            void setupBuffers();

            std::unique_ptr<VertexArray> skyboxVao;
            std::unique_ptr<VertexBuffer> skyboxVbo;
            std::unique_ptr<IndexBuffer> skyboxIbo;

            Shader* cubemapShader;

        private:
            Texture* cubemapTexture;

            // Optional ability to rotate around y axis
            bool animated = false;
            float angleY = 0.0f;
            float rotationSpeed = 0.01f;
    };
}
