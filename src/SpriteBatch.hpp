#ifndef __SpriteBatch__
#define __SpriteBatch__

#include <GL/glew.h>
#include "Vertex.hpp"
#include "Texture.hpp"

#include "VertexArray.hpp"

#include <memory>
#include <vector>

namespace Villain {

    enum class GlyphSortType {
        NONE,
        FRONT_TO_BACK,
        BACK_TO_FRONT,
        TEXTURE
    };

    class Glyph {
        public:
            Glyph(const glm::vec4& destRect,
                    const glm::vec4& uvRect,
                    GLuint txt,
                    float dp,
                    const glm::vec4& color);

            Glyph(const glm::vec4& destRect,
                    const glm::vec4& uvRect,
                    GLuint txt,
                    float dp,
                    const glm::vec4& color,
                    float angle);

            GLuint texture;
            float depth;

            VertexC topLeft;
            VertexC bottomLeft;
            VertexC topRight;
            VertexC bottomRight;
        private:
            glm::vec2 rotatePoint(glm::vec2 position, float angle);
    };

    class RenderBatch {
        public:
            RenderBatch (GLuint Offset, GLuint NumVertices, GLuint Texture) :
                offset(Offset), numVertices(NumVertices), texture(Texture) {}
            GLuint offset;
            GLuint numVertices;
            GLuint texture;
        private:
    };

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

            // 1st draw() overload - rotate by angle in radians around sprite origin
            void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const glm::vec4& color, float angleRad);

            // 2nd draw() overload - rotate by direction vector around sprite origin
            void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const glm::vec4& color, const glm::vec2& dir);

            // 3rd draw() overload - draw frame from texture atlas
            void draw(const glm::vec4& destRect, int frame, int row, int width, int height, Texture* texture, float depth, const glm::vec4& color);

            void draw(const glm::vec4& destRect, int frame, int row, int width, int height, Texture* texture, float depth, const glm::vec4& color, const glm::vec2& dir);
            void renderBatch();
        private:
            void createRenderBatches();
            void createVAO();
            void sortGlyphs();
            glm::vec4 measureUV(int column, int row, int width, int height, Texture* texture);

            // TODO: Use lambdas for these compare functions instead
            static bool compareFrontToBack(Glyph* a, Glyph* b);
            static bool compareBackToFront(Glyph* a, Glyph* b);
            static bool compareTexture(Glyph* a, Glyph* b);

            GlyphSortType sortType;

            std::vector<Glyph*> glyphPointers; //<<< For sorting glyphs
            std::vector<Glyph> glyphs; //<<< Actual Glyphs used in rendering
            std::vector<RenderBatch> renderBatches;

            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;

    };
}

#endif // __SpriteBatch__
