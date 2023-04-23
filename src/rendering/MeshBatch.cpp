#include "MeshBatch.hpp"

namespace Villain {
    MeshBatch::MeshBatch(Shader* shader): defaultShader(shader) {}

    MeshBatch::~MeshBatch() {}

    void MeshBatch::init() {
        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(nullptr, -1);
        ibo = std::make_unique<IndexBuffer>(nullptr, -1);
        VertexBufferLayout layout = VertexP1N1UV::getVertexLayout();
        vao->addBuffer(*vbo, layout);
    }

    void MeshBatch::begin(GlyphSortType sortType) {
        this->sortType = sortType;
        renderBatches.clear();
    }

    void MeshBatch::draw(Mesh<VertexP1N1UV>* mesh, Material& material, Transform& transform) {
        // TODO: add indices, vertices
        // Set uniforms where???

    }

    void MeshBatch::end() {

        // TODO: sort glyphs here, create render batches

        // Fill buffers with data
        vbo->fill(vertices.data(), vertices.size() * sizeof(VertexP1N1UV), GL_DYNAMIC_DRAW);
        vbo->unbind();

        ibo->fill(indices.data(), indices.size() * sizeof(GLuint), GL_DYNAMIC_DRAW);
        ibo->unbind();

        numElements = indices.size();
        indices.clear();
        vertices.clear();
    }

    void MeshBatch::renderBatch() {
        vao->bind();
        for (int i = 0; i < renderBatches.size(); i++) {
            // TODO: will need to bind all material textures here
            glBindTexture(GL_TEXTURE_2D, renderBatches[i].texture) ;

            glDrawArrays(GL_TRIANGLES, renderBatches[i].offset, renderBatches[i].numVertices);
        }
        vao->unbind();
    }

}
