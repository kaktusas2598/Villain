#ifndef __Mesh__
#define __Mesh__

#include <vector>
#include <memory>
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

class Mesh {
    public:
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
        std::vector<Texture*> Textures;

        glm::vec4 diffuseColor;

        Mesh(std::vector<Vertex> vertices,
                std::vector<unsigned int> indices,
                std::vector<Texture*> textures,
                glm::vec4 diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        void draw(Shader &shader);
        // TODO: delete textures in here?
        // ~Mesh() {}
    private:
        //unsigned int VBO, VAO, EBO;
        std::unique_ptr<VertexArray> vao;
        std::unique_ptr<VertexBuffer> vbo;
        std::unique_ptr<IndexBuffer> ibo;


        void setupMesh();
};

#endif // __Mesh__
