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
        renderBatches.clear();

        for (int i = 0; i < glyphs.size(); i++) {
            delete glyphs[i];
        }
        glyphs.clear();
    }

    void SpriteBatch::end() {
        sortGlyphs();
        createRenderBatches();
    }

    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const glm::vec4& color) {
        // Add a glyph
        Glyph* newGlyph = new Glyph();
        newGlyph->texture = texture;
        newGlyph->depth = depth;

        newGlyph->topLeft.Color = color;
        newGlyph->topLeft.Position = glm::vec3(destRect.x, destRect.y + destRect.w, depth);
        newGlyph->topLeft.UV = glm::vec2(uvRect.x, uvRect.y + uvRect.w);

        newGlyph->bottomLeft.Color = color;
        newGlyph->bottomLeft.Position = glm::vec3(destRect.x, destRect.y, depth);
        newGlyph->bottomLeft.UV = glm::vec2(uvRect.x, uvRect.y);

        newGlyph->topRight.Color = color;
        newGlyph->topRight.Position = glm::vec3(destRect.x + destRect.z, destRect.y + destRect.w, depth);
        newGlyph->topRight.UV = glm::vec2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

        newGlyph->bottomRight.Color = color;
        newGlyph->bottomRight.Position = glm::vec3(destRect.x + destRect.z, destRect.y, depth);
        newGlyph->bottomRight.UV = glm::vec2(uvRect.x + uvRect.z, uvRect.y);

        glyphs.push_back(newGlyph);
    }

    // HACK: This method is only a proof of concept for drawing sprite from a spritesheet, will need to be improved a lot
    void SpriteBatch::draw(const glm::vec4& destRect, int frame, int row, int width, int height, Texture* texture, float depth, const glm::vec4& color) {
        glm::vec4 uvRect;
        // 0 - 1, where 1 is texture width
        uvRect.x = frame * 1/(texture->getWidth()/(float) width);
        uvRect.y = row * 1/(texture->getHeight()/(float) height);
        uvRect.z = 1/(texture->getWidth() / (float) width);
        uvRect.w = 1/(texture->getHeight() / (float) height);

        draw(destRect, uvRect, texture->getID(), depth, color);
    }

    void SpriteBatch::renderBatch() {
        glBindVertexArray(vao);
        for (int i = 0; i < renderBatches.size(); i++) {
           glBindTexture(GL_TEXTURE_2D, renderBatches[i].texture) ;

           glDrawArrays(GL_TRIANGLES, renderBatches[i].offset, renderBatches[i].numVertices);
        }
        glBindVertexArray(0);
    }

    void SpriteBatch::createRenderBatches() {
        std::vector <VertexC> vertices;
        vertices.resize(glyphs.size() * 6);

        if (glyphs.empty()) {
            return;
        }

        int offset = 0;
        int currentVertex = 0;
        renderBatches.emplace_back(offset, 6, glyphs[0]->texture);
        vertices[currentVertex++] = glyphs[0]->topLeft;
        vertices[currentVertex++] = glyphs[0]->bottomLeft;
        vertices[currentVertex++] = glyphs[0]->bottomRight;
        vertices[currentVertex++] = glyphs[0]->bottomRight;
        vertices[currentVertex++] = glyphs[0]->topRight;
        vertices[currentVertex++] = glyphs[0]->topLeft;
        offset += 6;

        for (int currentGlyph = 1; currentGlyph < glyphs.size(); currentGlyph++) {
            if (glyphs[currentGlyph]->texture != glyphs[currentGlyph - 1]->texture) {
                renderBatches.emplace_back(offset, 6, glyphs[currentGlyph]->texture);
            } else {
                renderBatches.back().numVertices += 6;
            }
            vertices[currentVertex++] = glyphs[currentGlyph]->topLeft;
            vertices[currentVertex++] = glyphs[currentGlyph]->bottomLeft;
            vertices[currentVertex++] = glyphs[currentGlyph]->bottomRight;
            vertices[currentVertex++] = glyphs[currentGlyph]->bottomRight;
            vertices[currentVertex++] = glyphs[currentGlyph]->topRight;
            vertices[currentVertex++] = glyphs[currentGlyph]->topLeft;
            offset += 6;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // orphan the buffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexC), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(VertexC), vertices.data());

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void SpriteBatch::createVAO() {
        if (vao == 0)
            glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        if (vbo == 0)
            glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexC), (void*)0); // Position
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexC), (void*)(sizeof(float) * 3)); // Color
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexC), (void*)(sizeof(float) * 7)); // UV

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
