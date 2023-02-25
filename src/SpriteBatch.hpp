#ifndef __SpriteBatch__
#define __SpriteBatch__

#include <GL/glew.h>
#include "Vertex.hpp"
#include <vector>

namespace Villain {

    enum class GlyphSortType {
        NONE,
        FRONT_TO_BACK,
        BACK_TO_FRONT,
        TEXTURE
    };

    struct Glyph {
        GLuint texture;
        float depth;

        VertexC topLeft;
        VertexC bottomLeft;
        VertexC topRight;
        VertexC bottomRight;
    };

    class RenderBatch {
        public:
            RenderBatch (GLuint Offset, GLuint NumVertices, GLuint Texture) :
                offset(Offset), numVertices(NumVertices), texture(Texture) {}
            //~RenderBatch ();
            GLuint offset;
            GLuint numVertices;
            GLuint texture;
        private:
    };

    //RenderBatch ::~RenderBatch ()
    //{
    //}

    class SpriteBatch {
        public:
            SpriteBatch();
            ~SpriteBatch();

            void init();

            void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);
            void end();

            /*
             * vec.x
             * vec.y
             * vec.z = width
             * vec.w = height
             */
            void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const glm::vec4& color);

            void renderBatch();
        private:
            void createRenderBatches();
            void createVAO();
            void sortGlyphs();

            // TODO: Use lambdas for these compare functions instead
            static bool compareFrontToBack(Glyph* a, Glyph* b);
            static bool compareBackToFront(Glyph* a, Glyph* b);
            static bool compareTexture(Glyph* a, Glyph* b);

            GLuint vbo;
            GLuint vao;

            GlyphSortType sortType;

            std::vector<Glyph*> glyphs;
            std::vector<RenderBatch> renderBatches;
    };
}

#endif // __SpriteBatch__
