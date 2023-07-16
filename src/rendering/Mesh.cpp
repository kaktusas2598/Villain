#include "Mesh.hpp"

#include "Renderer.hpp"
#include "ErrorHandler.hpp"
#include "VertexBufferLayout.hpp"
#include <cstring>

namespace Villain {

    //Mesh<VertexType>::Mesh(std::vector<VertexType> vertices, std::vector<unsigned int> indices) {
    template <class VertexType>
    Mesh<VertexType>::Mesh(std::vector<VertexType> vertices, std::vector<unsigned int> indices, unsigned instances, std::vector<glm::mat4>instanceTransforms) {
        Vertices = vertices;
        Indices = indices;
        numInstances = instances;
        instanceMatrix = instanceTransforms;

        setupMesh();
    }

    template <class VertexType>
    Mesh<VertexType>::Mesh(
                    std::vector<VertexType> vertices,
                    std::vector<unsigned int> indices,
                    const std::string& matName,
                    unsigned instances,
                    std::vector<glm::mat4> instanceTransforms) {
        Vertices = vertices;
        Indices = indices;
        materialName = matName;
        numInstances = instances;
        instanceMatrix = instanceTransforms;

        setupMesh();
    }

    template <class VertexType>
    void Mesh<VertexType>::draw(Shader &shader, Material& material) {
        shader.setMaterialUniforms(material);
        if (numInstances > 1)
            shader.setUniform1i("instancedRenderingEnabled", true);
        else
            shader.setUniform1i("instancedRenderingEnabled", false);

        Renderer renderer;
        renderer.draw(*vao, *ibo, shader, numInstances);
    }

    template <class VertexType>
    void Mesh<VertexType>::setupMesh() {
        // Create bounding volime
        boundingVolume = std::make_unique<BoundingSphere>(generateSphereBV<VertexType>(*this));

        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(Vertices.data(), Vertices.size() * sizeof(VertexType));

        // Use template argument to get correct vertex attribute layout
        VertexBufferLayout layout = VertexType::getVertexLayout();
        vao->addBuffer(*vbo, layout);

        // Instanced drawing mat4 layout setup with custom VBO
        if (numInstances > 1) {
            setupInstancedAttributes();
        }

        // Generate and bind index buffer object
        ibo = std::make_unique<IndexBuffer>(Indices.data(), Indices.size());
    }

    template <class VertexType>
    void Mesh<VertexType>::setupInstancedAttributes() {
        vao->bind();
        unsigned attributeLocation = VertexType::getVertexLayout().getNumOfElements();
        instanceVbo = std::make_unique<VertexBuffer>(instanceMatrix.data(), sizeof(glm::mat4) * instanceMatrix.size());

        std::size_t vec4Size = sizeof(glm::vec4);
        // Vertex attributes pointers can only be up to vec4 in size, so we split mat4 into 4 vertex attribs
        GLCall(glEnableVertexAttribArray(attributeLocation));
        GLCall(glVertexAttribPointer(attributeLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0));
        GLCall(glEnableVertexAttribArray(attributeLocation + 1));
        GLCall(glVertexAttribPointer(attributeLocation + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4))));
        GLCall(glEnableVertexAttribArray(attributeLocation + 2));
        GLCall(glVertexAttribPointer(attributeLocation + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4))));
        GLCall(glEnableVertexAttribArray(attributeLocation + 3));
        GLCall(glVertexAttribPointer(attributeLocation + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4))));

        // Telling OpenGL that these attributes belong to single instance
        GLCall(glVertexAttribDivisor(attributeLocation + 0, 1));
        GLCall(glVertexAttribDivisor(attributeLocation + 1, 1));
        GLCall(glVertexAttribDivisor(attributeLocation + 2, 1));
        GLCall(glVertexAttribDivisor(attributeLocation + 3, 1));
    }

    template <class VertexType>
    void Mesh<VertexType>::updateInstances(std::vector<glm::mat4>& instances) {
        // Assuming mesh is already instanced
        if (numInstances == 1) {
            setupInstancedAttributes();
        }

        instanceVbo->bind();

        // Check if the size needs to be changed
        if (instances.size() != numInstances) {
            // Resize the buffer object
            instanceVbo->fill(instances.data(), sizeof(glm::mat4) * instances.size());

            numInstances = instances.size();
        } else {
            // Map the buffer data for writing
            GLvoid* instanceData = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            if (instanceData != nullptr) {
                // Copy the updated instance matrices to the mapped memory
                std::memcpy(instanceData, instances.data(), sizeof(glm::mat4) * instances.size());

                // Unmap the buffer
                GLCall(glUnmapBuffer(GL_ARRAY_BUFFER));
            }
        }

        instanceVbo->unbind();
    }

    // Explicit instantiation of template specialisations to avoid linker errors, alternativaly and even better
    // template should be defined in header file only
    template Mesh<VertexP1>::Mesh(std::vector<VertexP1> vertices, std::vector<unsigned int> indices, unsigned instances, std::vector<glm::mat4> instanceTransforms);
    template Mesh<VertexP1UV>::Mesh(std::vector<VertexP1UV> vertices, std::vector<unsigned int> indices, unsigned instances, std::vector<glm::mat4> instanceTransforms);
    template Mesh<VertexP1N1>::Mesh(std::vector<VertexP1N1> vertices, std::vector<unsigned int> indices, unsigned instances, std::vector<glm::mat4> instanceTransforms);
    template Mesh<VertexP1N1UV>::Mesh(std::vector<VertexP1N1UV> vertices, std::vector<unsigned int> indices, unsigned instances, std::vector<glm::mat4> instanceTransforms);
    template Mesh<VertexP1N1T1B1UV>::Mesh(std::vector<VertexP1N1T1B1UV> vertices, std::vector<unsigned int> indices, unsigned instances, std::vector<glm::mat4> instanceTransforms);
    template Mesh<VertexP1C1UV>::Mesh(std::vector<VertexP1C1UV> vertices, std::vector<unsigned int> indices, unsigned instances, std::vector<glm::mat4> instanceTransforms);
    template Mesh<VertexP1N1C1UV>::Mesh(std::vector<VertexP1N1C1UV> vertices, std::vector<unsigned int> indices, unsigned instances, std::vector<glm::mat4> instanceTransforms);

    template Mesh<VertexP1>::Mesh(std::vector<VertexP1> vertices, std::vector<unsigned int> indices, const std::string& matName, unsigned, std::vector<glm::mat4>);
    template Mesh<VertexP1UV>::Mesh(std::vector<VertexP1UV> vertices, std::vector<unsigned int> indices, const std::string& matName, unsigned, std::vector<glm::mat4>);
    template Mesh<VertexP1N1>::Mesh(std::vector<VertexP1N1> vertices, std::vector<unsigned int> indices, const std::string& matName, unsigned, std::vector<glm::mat4>);
    template Mesh<VertexP1N1UV>::Mesh(std::vector<VertexP1N1UV> vertices, std::vector<unsigned int> indices, const std::string& matName, unsigned, std::vector<glm::mat4>);
    template Mesh<VertexP1N1T1B1UV>::Mesh(std::vector<VertexP1N1T1B1UV> vertices, std::vector<unsigned int> indices, const std::string& matName, unsigned, std::vector<glm::mat4>);
    template Mesh<VertexP1C1UV>::Mesh(std::vector<VertexP1C1UV> vertices, std::vector<unsigned int> indices, const std::string& matName, unsigned, std::vector<glm::mat4>);
    template Mesh<VertexP1N1C1UV>::Mesh(std::vector<VertexP1N1C1UV> vertices, std::vector<unsigned int> indices, const std::string& matName, unsigned, std::vector<glm::mat4>);

    template void Mesh<VertexP1>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1UV>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1N1>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1N1UV>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1C1UV>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1N1C1UV>::draw(Shader &shader, Material& material);
    template void Mesh<VertexP1N1T1B1UV>::draw(Shader &shader, Material& material);

    template void Mesh<VertexP1>::updateInstances(std::vector<glm::mat4>&);
    template void Mesh<VertexP1UV>::updateInstances(std::vector<glm::mat4>&);
    template void Mesh<VertexP1N1>::updateInstances(std::vector<glm::mat4>&);
    template void Mesh<VertexP1N1UV>::updateInstances(std::vector<glm::mat4>&);
    template void Mesh<VertexP1C1UV>::updateInstances(std::vector<glm::mat4>&);
    template void Mesh<VertexP1N1C1UV>::updateInstances(std::vector<glm::mat4>&);
    template void Mesh<VertexP1N1T1B1UV>::updateInstances(std::vector<glm::mat4>&);

}
