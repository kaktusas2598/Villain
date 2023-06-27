#ifndef __Mesh__
#define __Mesh__

#include <vector>
#include <memory>
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Material.hpp"
#include "Shader.hpp"

#include "Frustum.hpp"

namespace Villain {

    template <class VertexType> class Mesh {
        public:
            std::vector<VertexType> Vertices;
            std::vector<unsigned int> Indices;

            // Passing more than 1 instances enables instanced rendering and instanceMatrices vector must be provided
            Mesh(
                    std::vector<VertexType> vertices,
                    std::vector<unsigned int> indices,
                    unsigned instances = 1,
                    std::vector<glm::mat4> instanceTransforms = {});
            Mesh(
                    std::vector<VertexType> vertices,
                    std::vector<unsigned int> indices,
                    const std::string& matName,
                    unsigned instances = 1,
                    std::vector<glm::mat4> instanceTransforms = {});

            void draw(Shader &shader);
            void draw(Shader &shader, Material& material);

            Sphere* getBoundingVolume() { return boundingVolume.get(); }
            const std::string& getMaterialName() const { return materialName; }

            bool isInstanced() const { return numInstances > 1; }
        private:
            std::string materialName = std::string();
            //unsigned int VBO, VAO, EBO;
            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;

            std::unique_ptr<Sphere> boundingVolume;

            // Instanced rendering properties
            unsigned int numInstances = 1; //<<< Set to more than 1 instance for instanced rendering
            std::vector<glm::mat4> instanceMatrix;
            std::unique_ptr<VertexBuffer> instanceVbo;

            void setupMesh();
    };

    // Helper method to generate Sphere shaped bounding volume to fit mesh in
    template <class VertexType>
    Sphere generateSphereBV(const Mesh<VertexType>& mesh) {
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
        for (auto&& vertex : mesh.Vertices) {
            minAABB.x = std::min(minAABB.x, vertex.Position.x);
            minAABB.y = std::min(minAABB.y, vertex.Position.y);
            minAABB.z = std::min(minAABB.z, vertex.Position.z);

            maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
            maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
            maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
        }

        //std::cout << "maxAABB: " << maxAABB.x << ", " << maxAABB.y << ", " << maxAABB.z << "\n";
        //std::cout << "minAABB: " << minAABB.x << ", " << minAABB.y << ", " << minAABB.z << "\n";
        return Sphere((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
    }

    // Helper method to generate AABB shaped bounding volume to fit mesh in
    template <class VertexType>
    AABB generateAABoxBV(const Mesh<VertexType>& mesh) {
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
        for (auto&& vertex : mesh.Vertices) {
            minAABB.x = std::min(minAABB.x, vertex.Position.x);
            minAABB.y = std::min(minAABB.y, vertex.Position.y);
            minAABB.z = std::min(minAABB.z, vertex.Position.z);

            maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
            maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
            maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
        }

        return AABB(minAABB, maxAABB);
    }

}

#endif // __Mesh__
