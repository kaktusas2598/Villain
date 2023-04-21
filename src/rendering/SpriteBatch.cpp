#include "SpriteBatch.hpp"
#include "VertexBufferLayout.hpp"
#include "glm/geometric.hpp"

#include <algorithm>
#include <type_traits>

#include <iostream>

namespace Villain {

    Glyph::Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint txt, float dp, const glm::vec4& color) {
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

    Glyph::Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint txt, float dp, const glm::vec4& color, float angle) {
        texture = txt;
        depth = dp;

        glm::vec2 halfDimensions(destRect.z / 2.0f, destRect.w / 2.0f);
        // Get points centered around origin
        glm::vec2 tl(-halfDimensions.x, halfDimensions.y); //top left
        glm::vec2 bl(-halfDimensions.x, -halfDimensions.y); //bottom left
        glm::vec2 br(halfDimensions.x, -halfDimensions.y); //bottom right
        glm::vec2 tr(halfDimensions.x, halfDimensions.y); //top right

        tl = rotatePoint(tl, angle) + halfDimensions;
        bl = rotatePoint(bl, angle) + halfDimensions;
        br = rotatePoint(br, angle) + halfDimensions;
        tr = rotatePoint(tr, angle) + halfDimensions;

        topLeft.Color = color;
        topLeft.Position = glm::vec3(destRect.x + tl.x, destRect.y + tl.y, depth);
        topLeft.UV = glm::vec2(uvRect.x, uvRect.y + uvRect.w);

        bottomLeft.Color = color;
        bottomLeft.Position = glm::vec3(destRect.x + bl.x, destRect.y + bl.y, depth);
        bottomLeft.UV = glm::vec2(uvRect.x, uvRect.y);

        topRight.Color = color;
        topRight.Position = glm::vec3(destRect.x + tr.x, destRect.y + tr.y, depth);
        topRight.UV = glm::vec2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

        bottomRight.Color = color;
        bottomRight.Position = glm::vec3(destRect.x + br.x, destRect.y + br.y, depth);
        bottomRight.UV = glm::vec2(uvRect.x + uvRect.z, uvRect.y);
    }

    glm::vec2 Glyph::rotatePoint(glm::vec2 position, float angle) {
        glm::vec2 newPos;
        newPos.x = position.x * cos(angle) - position.y * sin(angle);
        newPos.y = position.x * sin(angle) + position.y * cos(angle);
        return newPos;
    }


    SpriteBatch::SpriteBatch() { }

    SpriteBatch::~SpriteBatch() { }

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

    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const glm::vec4& color, float angleRad) {
        glyphs.emplace_back(destRect, uvRect, texture, depth, color, angleRad);
    }

    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const glm::vec4& color, const glm::vec2& dir) {
        // assume for now sprites will face right by default, so positive X only in 2D ortho
        const glm::vec2 right(1.0f, 0.0f);
        float angle = acos(glm::dot(right, dir));
        if (dir.y < 0.0f) angle = -angle;

        glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle);
    }

    void SpriteBatch::draw(const glm::vec4& destRect, int frame, int row, int width, int height, Texture* texture, float depth, const glm::vec4& color) {
        glm::vec4 uvRect = measureUV(frame, row, width, height, texture);
        draw(destRect, uvRect, texture->getID(), depth, color);
    }

    void SpriteBatch::draw(const glm::vec4& destRect, int frame, int row, int width, int height, Texture* texture, float depth, const glm::vec4& color, const glm::vec2& dir) {
        glm::vec4 uvRect = measureUV(frame, row, width, height, texture);

        // assume for now sprites will face right by default, so positive X only in 2D ortho
        const glm::vec2 right(1.0f, 0.0f);
        float angle = acos(glm::dot(right, dir));
        if (dir.y < 0.0f) angle = -angle;

        draw(destRect, uvRect, texture->getID(), depth, color, angle);
    }

    glm::vec4 SpriteBatch::measureUV(int column, int row, int width, int height, Texture* texture) {
        glm::vec4 uvRect;
        // 0 - 1, where 1 is texture width
        // Calculate num rows to flip actual row because loaded textures are flipped vertically in STBI image
        int numRows = texture->getHeight() / height;
        int numColumns = texture->getWidth() / width;
        uvRect.x = column * 1.0f/numColumns;
        uvRect.y = (numRows - row - 1) * 1.0f/numRows;
        //uvRect.y = row * 1.0f/numRows;
        //std::cout << "Num rows: " << numRows << ", Num columns: " << numColumns << "\n";

        uvRect.z = 1.0f/numColumns;
        uvRect.w = 1.0f/numRows;

        return uvRect;
    }

    void SpriteBatch::renderBatch() {
        vao->bind();
        for (int i = 0; i < renderBatches.size(); i++) {
           glBindTexture(GL_TEXTURE_2D, renderBatches[i].texture) ;

           glDrawArrays(GL_TRIANGLES, renderBatches[i].offset, renderBatches[i].numVertices);
        }
        vao->unbind();
    }

    void SpriteBatch::createRenderBatches() {
        std::vector <VertexP1C1UV> vertices;
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


        vbo->fill(vertices.data(), vertices.size() * sizeof(VertexP1C1UV), GL_DYNAMIC_DRAW);
        vbo->unbind();
    }

    void SpriteBatch::createVAO() {
        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(nullptr, -1);
        VertexBufferLayout layout = VertexP1C1UV::getVertexLayout();
        vao->addBuffer(*vbo, layout);
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