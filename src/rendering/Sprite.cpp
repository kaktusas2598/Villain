#include "Sprite.hpp"

#include "Renderer.hpp"
#include "ResourceManager.hpp"

#include <iostream>

namespace Villain {

    Sprite::Sprite(const std::string& textureID, const std::string& shaderID) {
        texture = ResourceManager::Instance()->getTexture(textureID);
        shader = ResourceManager::Instance()->getShader(shaderID);
    }

    Sprite::~Sprite() {}

    void Sprite::init(int x, int y, int width, int height) {
        position.x = x;
        position.y = y;
        position.z = 0.0f;
        this->width = width;
        this->height = height;

        setup();
    }
    void Sprite::init(int x, int y, int z, int width, int height) {
        position.x = x;
        position.y = y;
        position.z = z;
        this->width = width;
        this->height = height;

        setup();
    }

    void Sprite::setup() {
        // position, texCoords
        float vertices[] = {
            position.x + width, position.y + height, position.z, 1.0f, 1.0f,
            position.x, position.y + height, position.z, 0.0f, 1.0f,
            position.x, position.y, position.z, 0.0f, 0.0f,
            position.x + width, position.y, position.z, 1.0f, 0.0f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };
        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(vertices, 4 * 5 * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(2); // texCoords

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(indices, 6);
    }

    void Sprite::update(float deltaTime) {}
    void Sprite::draw() {
        texture->bind();
        Renderer::Instance()->draw(*vao, *ibo, *const_cast<Shader*>(shader));
    }

    //glm::vec3 position;
    //int width;
    //int height;

    //Vertex vertices[4];

    //std::unique_ptr<VertexArray> vao;
    //std::unique_ptr<VertexBuffer> vbo;
    //std::unique_ptr<IndexBuffer> ibo;
    //std::unique_ptr<Shader> shader;
    //std::unique_ptr<Texture> texture;
}

