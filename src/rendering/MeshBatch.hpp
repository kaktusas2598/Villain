#ifndef __MESH__BATCH__
#define __MESH__BATCH__

#include "Mesh.hpp"
// Reusing some stugg from original 2D sprite batch
#include "SpriteBatch.hpp"

namespace Villain {

    struct MeshBatchConfig {
        unsigned int RenderType = GL_TRIANGLES;
        // NOTE: this will have to be ordered by distance to camera and rendered back to front
        unsigned int Priority = 1; // Lower priority batches get rendered first
        unsigned int TextureID = 0; // Optional texture
        glm::mat4 TransformMatrix = glm::mat4(1.0f); // Model/world transform matrix

        MeshBatchConfig(unsigned renderType = GL_TRIANGLES, unsigned priority = 1, unsigned texture = 0) :
            RenderType(renderType), Priority(priority), TextureID(texture) {}

        bool operator==(const MeshBatchConfig& other) const {
            if (RenderType != other.RenderType || Priority != other.Priority
                    || TextureID != other.TextureID || TransformMatrix != other.TransformMatrix) {
                return false;
            }
            return true;
        }

        bool operator!=(const MeshBatchConfig& other) const {
            return !(*this == other);
        }
    };

    // This is a complete experimental work, trying to implement 3D Mesh Batch Renderer
    // The goal is to get way more FPS with big 3D models like sponza and this will
    // take quite a while to implement
    // TODO: FEATURE LIST:
    // 1: Instead of 1 texture each render batch should be able to store multiple textures
    // and assign them to different sampling slots: diffuse, specular, normal etc.
    // 2: ability to render using different shaders
    // 3. uniforms?? if betch gets rendered once per frame then at least MVP matrices should not change
    // 4. Could possible be a template class? But when we need to split batches by Vertex type as well
    // 5. Batches ordered by texture, shader, trnasform uniform and meshes position from camera (rendered from back to front)
    class MeshBatch {
        public:
            //MeshBatch(Shader* shader);
            MeshBatch(unsigned int maxVert, Shader* shader);
            ~MeshBatch();

            bool isBatchConfig(const MeshBatchConfig& cfg) const { return config == cfg; }
            bool isEmpty() const { return usedVertices == 0; }
            bool isEnoughRoom(unsigned numVertices) const;

            MeshBatch* getFullest(MeshBatch* batch);
            int getPriority() const { return config.Priority; }

            //void add(const std::vector& vertices, const MeshBatchConfig& config);
            //void add(const std::vector& vertices);
            void render();

            void init();

            void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);
            void end();

            void draw(Mesh<VertexP1N1UV>* mesh, Material& material, Transform& transform);

            // NOTE: pass camera uniforms here??
            void renderBatch();
        private:
            //void createRenderBatches();
            //void createVAO();
            //void sortGlyphs();

            // TODO: Use lambdas for these compare functions instead
            //static bool compareFrontToBack(Glyph* a, Glyph* b);
            //static bool compareBackToFront(Glyph* a, Glyph* b);
            //static bool compareTexture(Glyph* a, Glyph* b);

            GlyphSortType sortType;

            //std::vector<Glyph*> glyphPointers; //<<< For sorting glyphs
            //std::vector<Glyph> glyphs; //<<< Actual Glyphs used in rendering
            std::vector<RenderBatch> renderBatches;

            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;

            std::vector<VertexP1N1UV> vertices;
            std::vector<GLuint> indices;
            int numElements = 0;
            unsigned maxVertices;
            unsigned usedVertices;
            MeshBatchConfig config;
            VertexP1N1UV lastVertex;

            Shader* defaultShader = nullptr;
    };
}

#endif // __MESH__BATCH__
