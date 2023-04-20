#include "Mesh.hpp"

#include "Renderer.hpp"
#include "ErrorHandler.hpp"
#include "VertexBufferLayout.hpp"

namespace Villain {

    template <class VertexType>
    Mesh<VertexType>::Mesh(std::vector<VertexType> vertices, std::vector<unsigned int> indices) {
        Vertices = vertices;
        Indices = indices;

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
    void Mesh<VertexType>::draw(Shader &shader, Material& material) {
        shader.setMaterialUniforms(material);

        Renderer renderer;
        renderer.draw(*vao, *ibo, shader);
    }

    template <class VertexType>
    void Mesh<VertexType>::setupMesh() {
        // Create bounding volime
        boundingVolume = std::make_unique<Sphere>(generateSphereBV<VertexType>(*this));

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
    template Mesh<VertexP1>::Mesh(std::vector<VertexP1> vertices, std::vector<unsigned int> indices);
    template Mesh<VertexP1UV>::Mesh(std::vector<VertexP1UV> vertices, std::vector<unsigned int> indices);
    template Mesh<VertexP1N1>::Mesh(std::vector<VertexP1N1> vertices, std::vector<unsigned int> indices);
    template Mesh<VertexP1N1UV>::Mesh(std::vector<VertexP1N1UV> vertices, std::vector<unsigned int> indices);
    template Mesh<VertexP1N1T1B1UV>::Mesh(std::vector<VertexP1N1T1B1UV> vertices, std::vector<unsigned int> indices);
    template Mesh<VertexP1C1UV>::Mesh(std::vector<VertexP1C1UV> vertices, std::vector<unsigned int> indices);
    template Mesh<VertexP1N1C1UV>::Mesh(std::vector<VertexP1N1C1UV> vertices, std::vector<unsigned int> indices);

    template Mesh<VertexP1>::Mesh(std::vector<VertexP1> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1UV>::Mesh(std::vector<VertexP1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1N1>::Mesh(std::vector<VertexP1N1> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1N1UV>::Mesh(std::vector<VertexP1N1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1N1T1B1UV>::Mesh(std::vector<VertexP1N1T1B1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1C1UV>::Mesh(std::vector<VertexP1C1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);
    template Mesh<VertexP1N1C1UV>::Mesh(std::vector<VertexP1N1C1UV> vertices, std::vector<unsigned int> indices, const std::string& matName);

    template void Mesh<VertexP1>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1UV>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1N1>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1N1UV>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1C1UV>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1N1C1UV>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1N1T1B1UV>::draw(Shader &shader, Material& material);
}
