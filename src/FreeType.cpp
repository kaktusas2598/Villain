#include "FreeType.hpp"

#include "rendering/Texture.hpp"
#include "rendering/SpriteBatch.hpp"
#include <iostream>

namespace Villain {

    FreeType::FreeType(const char* font, int size) {

        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        }

        FT_Face face;
        if (FT_New_Face(ft, font, 0, &face)) {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        }

        // Set font size
        FT_Set_Pixel_Sizes(face, 0, size);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        for (unsigned char c = 0; c < 128; c++) {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                    );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                //face->glyph->advance.x //ORIGINAL line
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            characters.insert(std::pair<char, Character>(c, character));
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void FreeType::draw(SpriteBatch& batch, std::string text, glm::vec2 position, float scale, float depth, glm::vec4 color) {
        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = characters[*c];

            float xpos = position.x + ch.Bearing.x * scale;
            float ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            // NOTE: added -1 factor to scale to fix vertically flipped glyphs
            float h = ch.Size.y * scale * -1;

            // Add each glyph to a sprite batch
            // NOTE: this could be made even faster by packing all glyphs into a single texture
            glm::vec4 destRect(xpos, ypos, w, h);
            glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

            batch.draw(destRect, uvRect, ch.TextureID, depth, color);
            //batch.draw(destRect, 0, 0, w, h, &glyphTexture, depth, color);

            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            position.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
    }
}
