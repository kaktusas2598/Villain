#include "Mesh.hpp"

#include "Renderer.hpp"
#include "ErrorHandler.hpp"
#include "VertexBufferLayout.hpp"

namespace Villain {

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, glm::vec4 diffuse) {
        Vertices = vertices;
        Indices = indices;
        Textures = textures;

        diffuseColor = diffuse;

        setupMesh();
    }

    void Mesh::draw(Shader &shader) {
        unsigned int diffuseIndex = 1;
        unsigned int specularIndex = 1;
        for (unsigned int i = 0; i < Textures.size(); i++) {
            Textures[i]->bind(i);
            std::string slot;
            std::string type = Textures[i]->getType();
            if (type == "texture_diffuse")
                slot = std::to_string(diffuseIndex++);
            else if (type == "texture_specular")
                slot = std::to_string(specularIndex++);

            // TODO: need to check if color set when loading mesh and pass color as uniform here as well
            shader.setUniform1i("material." + type + slot, i);
        }
        shader.setUniformVec4("material.diffuseColor", diffuseColor);

        Renderer renderer;
        renderer.draw(*vao, *ibo, shader);
    }

    void Mesh::setupMesh() {
        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(Vertices.data(), Vertices.size() * sizeof(Vertex));

        VertexBufferLayout layout;
        layout.push<float>(3); // position
        layout.push<float>(3); // normal
        layout.push<float>(2); // texCoords

        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(Indices.data(), Indices.size());

    }
}
