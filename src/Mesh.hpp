#ifndef __Mesh__
#define __Mesh__

#include <vector>
#include <memory>
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

namespace Villain {

    template <class VertexType> class Mesh {
        public:
            std::vector<VertexType> Vertices;
            std::vector<unsigned int> Indices;
            std::vector<Texture*> Textures;

            glm::vec4 diffuseColor;

            Mesh(std::vector<VertexType> vertices,
                    std::vector<unsigned int> indices,
                    std::vector<Texture*> textures,
                    glm::vec4 diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            // Instead of passing textures, pass material name
            Mesh(std::vector<VertexType> vertices, std::vector<unsigned int> indices, const std::string& matName);

            void draw(Shader &shader);
            void draw(Shader &shader, Material& material);
            // TODO: delete textures in here?
            // ~Mesh() {}

            const std::string& getMaterialName() const { return materialName; }
        private:
            std::string materialName = std::string();
            //unsigned int VBO, VAO, EBO;
            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;


            void setupMesh();
    };
}

#endif // __Mesh__
