#include "SpriteFont.hpp"

#include "SDL_ttf.h"
#include "SpriteBatch.hpp"

#include "SDL2/SDL.h"
#include <iostream>


int closestPow2(int i) {
    i--;
    int pi = 1;
    while (i > 0) {
        i >>= 1;
        pi <<= 1;
    }
    return pi;
}

#define MAX_TEXTURE_RES 4096

namespace Villain {

    SpriteFont::SpriteFont(const char* font, int size, char cs, char ce) {
        init(font, size, cs, ce);
    }

    SpriteFont::SpriteFont(const char* font, int size) {
        init(font, size, FIRST_PRINTABLE_CHAR, LAST_PRINTABLE_CHAR);
    }


    void SpriteFont::init(const char* font, int size) {
        init(font, size, FIRST_PRINTABLE_CHAR, LAST_PRINTABLE_CHAR);
    }

    void SpriteFont::init(const char* font, int size, char cs, char ce) {
        // Initialize SDL_ttf
        if (!TTF_WasInit()) {
            TTF_Init();
        }
        TTF_Font* f = TTF_OpenFont(font, size);
        if (f == nullptr) {
            std::cout << "Failed to open TTF font " << font << "\n";
            //fprintf(stderr, "Failed to open TTF font %s\n", font);
            //fflush(stderr);
            //throw 281;
        }
        fontHeight = TTF_FontHeight(f);
        std::cout << "Font height: " << fontHeight << "\n";
        regStart = cs;
        regLength = ce - cs + 1;
        int padding = size / 8;

        //std::cout << regStart << " " << regLength << std::endl;
        // First neasure all the regions
        glm::ivec4* glyphRects = new glm::ivec4[regLength];
        int i = 0, advance;
        for (char c = cs; c <= ce; c++) {
            TTF_GlyphMetrics(f, c, &glyphRects[i].x, &glyphRects[i].z, &glyphRects[i].y, &glyphRects[i].w, &advance);
            std::cout << glyphRects[i].x << ", " << glyphRects[i].y << ", " << glyphRects[i].z << ", " << glyphRects[i].w << ", " << advance << "\n";
            glyphRects[i].z -= glyphRects[i].x;
            glyphRects[i].x = 0;
            glyphRects[i].w -= glyphRects[i].y;
            glyphRects[i].y = 0;
            i++;
        }

        // Find best partitioning of glyphs
        int rows = 1, w, h, bestWidth = 0, bestHeight = 0, area = MAX_TEXTURE_RES * MAX_TEXTURE_RES, bestRows = 0;
        std::vector<int>* bestPartition = nullptr;
        while (rows <= regLength) {
            h = rows * (padding + fontHeight) + padding;
            auto gr = createRows(glyphRects, regLength, rows, padding, w);

            // Desire a power of 2 texture
            w = closestPow2(w);
            h = closestPow2(h);

            // A texture must be feasible
            if (w > MAX_TEXTURE_RES || h > MAX_TEXTURE_RES) {
                rows++;
                delete[] gr;
                continue;
            }

            // Check for minimal area
            if (area >= w * h) {
                if (bestPartition) delete[] bestPartition;
                bestPartition = gr;
                bestWidth = w;
                bestHeight = h;
                bestRows = rows;
                area = bestWidth * bestHeight;
                rows++;
            }
            else {
                delete[] gr;
                break;
            }
        }

        // Can a bitmap font be made?
        if (!bestPartition) {
            fprintf(stderr, "Failed to Map TTF font %s to texture. Try lowering resolution.\n", font);
            fflush(stderr);
            throw 282;
        }
        // Create the texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bestWidth, bestHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // Now draw all the glyphs
        SDL_Color fg = { 255, 255, 255, 255 };
        int ly = padding;
        for (int ri = 0; ri < bestRows; ri++) {
            int lx = padding;
            for (size_t ci = 0; ci < bestPartition[ri].size(); ci++) {
                int gi = bestPartition[ri][ci];

                SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(f, (char)(cs + gi), fg);
                if (glyphSurface == NULL) {
                    exitWithError("Failed to render glyph!");
                }

                // Pre-multiplication occurs here
                unsigned char* sp = (unsigned char*)glyphSurface->pixels;
                int cp = glyphSurface->w * glyphSurface->h * 4;
                for (int i = 0; i < cp; i += 4) {
                    float a = sp[i + 3] / 255.0f;
                    sp[i] = (unsigned char)((float)sp[i] * a);
                    sp[i + 1] = sp[i];
                    sp[i + 2] = sp[i];
                }

                // Save glyph image and update coordinates
                glTexSubImage2D(GL_TEXTURE_2D, 0, lx, bestHeight - ly - 1 - glyphSurface->h, glyphSurface->w, glyphSurface->h, GL_BGRA, GL_UNSIGNED_BYTE, glyphSurface->pixels);
                //glTexSubImage2D(GL_TEXTURE_2D, 0, lx, ly, glyphSurface->w, glyphSurface->h, GL_BGRA, GL_UNSIGNED_BYTE, glyphSurface->pixels);
                glyphRects[gi].x = lx;
                glyphRects[gi].y = ly;
                glyphRects[gi].z = glyphSurface->w;
                glyphRects[gi].w = glyphSurface->h;

                SDL_FreeSurface(glyphSurface);
                glyphSurface = nullptr;

                lx += glyphRects[gi].z + padding;
            }
            ly += fontHeight + padding;
        }

        // Draw the unsupported glyph
        int rs = padding - 1;
        int* pureWhiteSquare = new int[rs * rs];
        memset(pureWhiteSquare, 0xffffffff, rs * rs * sizeof(int));
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rs, rs, GL_RGBA, GL_UNSIGNED_BYTE, pureWhiteSquare);
        delete[] pureWhiteSquare;
        pureWhiteSquare = nullptr;

        // Set some texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Create spriteBatch glyphs
        glyphs = new CharGlyph[regLength + 1];
        for (i = 0; i < regLength; i++) {
            glyphs[i].character = (char)(cs + i);
            //std::cout << glyphs[i].character << std::endl;
            glyphs[i].size = glm::vec2(glyphRects[i].z, glyphRects[i].w);
            glyphs[i].uvRect = glm::vec4(
                (float)glyphRects[i].x / (float)bestWidth,
                (float)glyphRects[i].y / (float)bestHeight,
                (float)glyphRects[i].z / (float)bestWidth,
                (float)glyphRects[i].w / (float)bestHeight
                );
        }
        glyphs[regLength].character = ' ';
        glyphs[regLength].size = glyphs[0].size;
        glyphs[regLength].uvRect = glm::vec4(0, 0, (float)rs / (float)bestWidth, (float)rs / (float)bestHeight);

        glBindTexture(GL_TEXTURE_2D, 0);
        delete[] glyphRects;
        delete[] bestPartition;
        TTF_CloseFont(f);
    }

    void SpriteFont::dispose() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
        if (glyphs) {
            delete[] glyphs;
            glyphs = nullptr;
        }
    }

    std::vector<int>* SpriteFont::createRows(glm::ivec4* rects, int rectsLength, int r, int padding, int& w) {
        // Blank initialize
        std::vector<int>* l = new std::vector<int>[r]();
        int* cw = new int[r]();
        for (int i = 0; i < r; i++) {
            cw[i] = padding;
        }

        // Loop through all glyphs
        for (int i = 0; i < rectsLength; i++) {
            // Find row for placement
            int ri = 0;
            for (int rii = 1; rii < r; rii++)
                if (cw[rii] < cw[ri]) ri = rii;

            // Add width to that row
            cw[ri] += rects[i].z + padding;

            // Add glyph to the row list
            l[ri].push_back(i);
        }

        // Find the max width
        w = 0;
        for (int i = 0; i < r; i++) {
            if (cw[i] > w) w = cw[i];
        }

        return l;
    }

    glm::vec2 SpriteFont::measure(const char* s) {
        glm::vec2 size(0, fontHeight);
        float cw = 0;
        for (int si = 0; s[si] != 0; si++) {
            char c = s[si];
            if (s[si] == '\n') {
                size.y += fontHeight;
                if (size.x < cw)
                    size.x = cw;
                cw = 0;
            }
            else {
                // Check for correct glyph
                int gi = c - regStart;
                if (gi < 0 || gi >= regLength)
                    gi = regLength;
                cw += glyphs[gi].size.x;
            }
        }
        if (size.x < cw)
            size.x = cw;
        return size;
    }

    void SpriteFont::draw(SpriteBatch& batch, const char* s, glm::vec2 position, glm::vec2 scale,
        float depth, glm::vec4 tint, Justification just /* = Justification::LEFT */) {
        glm::vec2 tp = position;
        // Apply justification
        if (just == Justification::MIDDLE) {
            tp.x -= measure(s).x * scale.x / 2;
        }
        else if (just == Justification::RIGHT) {
            tp.x -= measure(s).x * scale.x;
        }
        for (int si = 0; s[si] != 0; si++) {
            char c = s[si];
            if (s[si] == '\n') {
                tp.y += fontHeight * scale.y;
                tp.x = position.x;
            }
            else {
                // Check for correct glyph
                int gi = c - regStart;
                if (gi < 0 || gi >= regLength) gi = regLength;
                glm::vec4 destRect(tp, glyphs[gi].size * scale);
                //if (c == '\u00E4') std::cout << glyphs[gi].uvRect.x << " " << glyphs[gi].uvRect.y << " " << glyphs[gi].uvRect.z << " " << glyphs[gi].uvRect.w << std::endl;
                batch.draw(destRect, glyphs[gi].uvRect, textureID, depth, tint);
                tp.x += glyphs[gi].size.x * scale.x;
            }
        }
    }
}
