#ifndef __FREETYPE__
#define __FREETYPE__

#include "glm/glm.hpp"
#include <map>
#include <string>

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
}

namespace Villain {

    class SpriteBatch;

    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    // Free Type Font Rendering
    class FreeType {
        public:
            FreeType(const char* font, int size);
            void draw(SpriteBatch& batch, std::string text, glm::vec2 position, float scale, float depth, glm::vec4 color);
        private:
            std::map<char, Character> characters;
    };
}

#endif // __FREETYPE__
