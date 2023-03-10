#include "SpriteBatch.hpp"

#include <algorithm>
#include <type_traits>

#include <iostream>

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
        glyphs.clear();
    }

    void SpriteBatch::end() {
        // Set up glyph pointers for fast sorting
        glyphPointers.resize(glyphs.size());
        for (int i = 0; i < glyphs.size(); i++) {
           glyphPointers[i] = &glyphs[i];
        }
        sortGlyphs();
        createRenderBatches();
    }

    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const glm::vec4& color) {
        glyphs.emplace_back(destRect, uvRect, texture, depth, color);
    }

    //HACK: This method is only a proof of concept for drawing sprite from a spritesheet, will need to be improved a lot
    void SpriteBatch::draw(const glm::vec4& destRect, int frame, int row, int width, int height, Texture* texture, float depth, const glm::vec4& color) {
        glm::vec4 uvRect;
        // 0 - 1, where 1 is texture width
        uvRect.x = frame * 1/(texture->getWidth()/(float) width);
        // Calculate num rows to flip actual row because loaded textures are flipped vertically in STBI image
        int numRows = texture->getHeight() / height;
        uvRect.y = (numRows - row - 1) * 1/(texture->getHeight()/(float) height);
        //uvRect.y = row * 1/(texture->getHeight()/(float) height);

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
        renderBatches.emplace_back(offset, 6, glyphPointers[0]->texture);
        vertices[currentVertex++] = glyphPointers[0]->topLeft;
        vertices[currentVertex++] = glyphPointers[0]->bottomLeft;
        vertices[currentVertex++] = glyphPointers[0]->bottomRight;
        vertices[currentVertex++] = glyphPointers[0]->bottomRight;
        vertices[currentVertex++] = glyphPointers[0]->topRight;
        vertices[currentVertex++] = glyphPointers[0]->topLeft;
        offset += 6;

        for (int currentGlyph = 1; currentGlyph < glyphPointers.size(); currentGlyph++) {
            if (glyphPointers[currentGlyph]->texture != glyphPointers[currentGlyph - 1]->texture) {
                renderBatches.emplace_back(offset, 6, glyphPointers[currentGlyph]->texture);
            } else {
                renderBatches.back().numVertices += 6;
            }
            vertices[currentVertex++] = glyphPointers[currentGlyph]->topLeft;
            vertices[currentVertex++] = glyphPointers[currentGlyph]->bottomLeft;
            vertices[currentVertex++] = glyphPointers[currentGlyph]->bottomRight;
            vertices[currentVertex++] = glyphPointers[currentGlyph]->bottomRight;
            vertices[currentVertex++] = glyphPointers[currentGlyph]->topRight;
            vertices[currentVertex++] = glyphPointers[currentGlyph]->topLeft;
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
                std::stable_sort(glyphPointers.begin(), glyphPointers.end(), compareBackToFront);
                break;
            case GlyphSortType::FRONT_TO_BACK:
                std::stable_sort(glyphPointers.begin(), glyphPointers.end(), compareFrontToBack);
                break;
            case GlyphSortType::TEXTURE:
                std::stable_sort(glyphPointers.begin(), glyphPointers.end(), compareTexture);
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
