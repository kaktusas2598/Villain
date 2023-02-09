#include "SpriteBatch.hpp"

#include <algorithm>
#include <type_traits>

namespace Villain {

    SpriteBatch::SpriteBatch(): vbo(0), vao(0) {
    }

    SpriteBatch::~SpriteBatch() {
    }

    void SpriteBatch::init() {
        createVAO();
    }

    void SpriteBatch::begin(GlyphSortType sortType) {
        this->sortType = sortType;
    }

    void SpriteBatch::end() {
        sortGlyphs();
    }

    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const glm::vec4& color) {
        // Add a glyph
        Glyph* newGlyph = new Glyph();
        newGlyph->texture = texture;
        newGlyph->depth = depth;

        newGlyph->topLeft.Color = color;
        newGlyph->topLeft.Position = glm::vec3(destRect.x, destRect.y + destRect.w, 0.0f);
        newGlyph->topLeft.UV = glm::vec2(uvRect.x, uvRect.y + uvRect.w);

        newGlyph->bottomLeft.Color = color;
        newGlyph->bottomLeft.Position = glm::vec3(destRect.x, destRect.y, 0.0f);
        newGlyph->bottomLeft.UV = glm::vec2(uvRect.x, uvRect.y);

        newGlyph->topRight.Color = color;
        newGlyph->topRight.Position = glm::vec3(destRect.x + destRect.z, destRect.y + uvRect.w, 0.0f);
        newGlyph->topRight.UV = glm::vec2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

        newGlyph->bottomRight.Color = color;
        newGlyph->bottomRight.Position = glm::vec3(destRect.x + destRect.z, destRect.y, 0.0f);
        newGlyph->bottomRight.UV = glm::vec2(uvRect.x + uvRect.z, uvRect.y);

        glyphs.push_back(newGlyph);
    }

    void SpriteBatch::renderBatch() {
    }

    void SpriteBatch::createRenderBatches() {
        //RenderBatch
    }

    void SpriteBatch::createVAO() {
        if (vao == 0)
            glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        if (vbo == 0)
            glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vao);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexC), (void*)0); // Position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexC), (void*)(sizeof(float) * 3)); // Color
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexC), (void*)(sizeof(float) * 6)); // UV

        glBindVertexArray(0);
    }

    void SpriteBatch::sortGlyphs() {
        switch (sortType) {
            case GlyphSortType::BACK_TO_FRONT:
                std::stable_sort(glyphs.begin(), glyphs.end(), compareBackToFront);
                break;
            case GlyphSortType::FRONT_TO_BACK:
                std::stable_sort(glyphs.begin(), glyphs.end(), compareFrontToBack);
                break;
            case GlyphSortType::TEXTURE:
                std::stable_sort(glyphs.begin(), glyphs.end(), compareTexture);
                break;
            default:
                break;
        }
    }

    bool SpriteBatch::compareFrontToBack(Glyph* a, Glyph* b) {
        return (a->depth < b->depth);
    }

    bool SpriteBatch::compareBackToFront(Glyph* a, Glyph* b) {
        return (a->depth > b->depth);
    }

    bool SpriteBatch::compareTexture(Glyph* a, Glyph* b) {
        return (a->texture < b->texture);
    }
}
