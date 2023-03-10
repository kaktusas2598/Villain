#ifndef __SpriteBatch__
#define __SpriteBatch__

#include <GL/glew.h>
#include "Vertex.hpp"
#include "Texture.hpp"
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
            Glyph() {}
            Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint txt, float dp, const glm::vec4& color) {
                texture = txt;
                depth = dp;

                topLeft.Color = color;
                topLeft.Position = glm::vec3(destRect.x, destRect.y + destRect.w, depth);
                topLeft.UV = glm::vec2(uvRect.x, uvRect.y + uvRect.w);

                bottomLeft.Color = color;
                bottomLeft.Position = glm::vec3(destRect.x, destRect.y, depth);
                bottomLeft.UV = glm::vec2(uvRect.x, uvRect.y);

                topRight.Color = color;
                topRight.Position = glm::vec3(destRect.x + destRect.z, destRect.y + destRect.w, depth);
                topRight.UV = glm::vec2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

                bottomRight.Color = color;
                bottomRight.Position = glm::vec3(destRect.x + destRect.z, destRect.y, depth);
                bottomRight.UV = glm::vec2(uvRect.x + uvRect.z, uvRect.y);
            }

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

            void draw(const glm::vec4& destRect, int frame, int row, int width, int height, Texture* texture, float depth, const glm::vec4& color);

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

            std::vector<Glyph*> glyphPointers; //<<< For sorting glyphs
            std::vector<Glyph> glyphs; //<<< Actual Glyphs used in rendering
            std::vector<RenderBatch> renderBatches;
    };
}

#endif // __SpriteBatch__
