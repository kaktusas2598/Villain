#pragma once

#include <memory>
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

namespace Villain {

    class Sprite {
        public:
            Sprite(const std::string& textureID, const std::string& shaderID);
            ~Sprite();

            void init(int x, int y, int width, int height);
            void init(int x, int y, int z, int width, int height);

            void update(float deltaTime);
            void draw();

        private:
            glm::vec3 position;
            int width;
            int height;

            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;

            Shader* shader;
            Texture* texture;

            void setup();
    };
}
