#include "Mesh.hpp"

#include "Renderer.hpp"
#include "ErrorHandler.hpp"
#include "VertexBufferLayout.hpp"

namespace Villain {

    template <class VertexType>
    Mesh<VertexType>::Mesh(std::vector<VertexType> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, glm::vec4 diffuse) {
        Vertices = vertices;
        Indices = indices;
        Textures = textures;

        diffuseColor = diffuse;

        setupMesh();
    }

    template <class VertexType>
    Mesh<VertexType>::Mesh(std::vector<VertexType> vertices, std::vector<unsigned int> indices, const std::string& matName) {
        Vertices = vertices;
        Indices = indices;
        materialName = matName;

        setupMesh();
    }

    template <class VertexType>
    void Mesh<VertexType>::draw(Shader &shader) {
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

    template <class VertexType>
    void Mesh<VertexType>::draw(Shader &shader, Material& material) {
        shader.setMaterialUniforms(material);

        Renderer renderer;
        renderer.draw(*vao, *ibo, shader);
    }

    template <class VertexType>
    void Mesh<VertexType>::setupMesh() {
        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(Vertices.data(), Vertices.size() * sizeof(VertexType));

        // Use template argument to get correct vertex attribute layout
        VertexBufferLayout layout = VertexType::getVertexLayout();
        vao->addBuffer(*vbo, layout);

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(Indices.data(), Indices.size());
    }

    // Explicit instantiation of template specialisations to avoid linker errors, alternativaly and even better
    // template should be defined in header file only
    template Mesh<VertexP1>::Mesh(std::vector<VertexP1> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, glm::vec4 diffuse);
    template Mesh<VertexP1UV>::Mesh(std::vector<VertexP1UV> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, glm::vec4 diffuse);
    template Mesh<VertexP1N1>::Mesh(std::vector<VertexP1N1> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, glm::vec4 diffuse);
    template Mesh<VertexP1N1UV>::Mesh(std::vector<VertexP1N1UV> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, glm::vec4 diffuse);
    template Mesh<VertexP1C1UV>::Mesh(std::vector<VertexP1C1UV> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, glm::vec4 diffuse);
    template Mesh<VertexP1N1C1UV>::Mesh(std::vector<VertexP1N1C1UV> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, glm::vec4 diffuse);

    template Mesh<VertexP1>::Mesh(std::vector<VertexP1> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1UV>::Mesh(std::vector<VertexP1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1N1>::Mesh(std::vector<VertexP1N1> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1N1UV>::Mesh(std::vector<VertexP1N1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1C1UV>::Mesh(std::vector<VertexP1C1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1N1C1UV>::Mesh(std::vector<VertexP1N1C1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);

    template void Mesh<VertexP1>::draw(Shader &shader);
    template void Mesh<VertexP1UV>::draw(Shader &shader);
    template void Mesh<VertexP1N1>::draw(Shader &shader);
    template void Mesh<VertexP1N1UV>::draw(Shader &shader);
    template void Mesh<VertexP1C1UV>::draw(Shader &shader);
    template void Mesh<VertexP1N1C1UV>::draw(Shader &shader);

    template void Mesh<VertexP1N1UV>::draw(Shader &shader, Material& material);
}
