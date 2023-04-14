/*
This is a modified version of the SpriteFont class from the
Seed Of Andromeda source code.
Use it for any of your projects, commercial or otherwise,
free of charge, but do not remove this disclaimer.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS
ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
OF THIS SOFTWARE.

November 28 2014
Original Author: Cristian Zaloj
Modified By: madvi11ain

FIXME: This class does not function well!
Event before modifying this it wasn't working, atm I am using my own font renderer
based on FreeType library, need to fix this one!
*/

#ifndef __SPRITEFONT__
#define __SPRITEFONT__

#include "SDL2/SDL_ttf.h"
#include "glm/glm.hpp"
#include <map>
#include <vector>

#include "Vertex.hpp"

namespace Villain {

    class SpriteBatch;

    struct CharGlyph {
        char character;
        glm::vec4 uvRect;
        glm::vec2 size;
    };

#define FIRST_PRINTABLE_CHAR ((char)32)
#define LAST_PRINTABLE_CHAR ((char)126)

    // Text allignment options
    enum class Justification {
        LEFT, MIDDLE, RIGHT
    };

    class SpriteFont {
        public:
            SpriteFont(const char* font, int size, char cs, char ce);
            SpriteFont(const char* font, int size);

            void init(const char* font, int size);
            void init(const char* font, int size, char cs, char ce);

            void dispose();

            int getFontHeight() const { return fontHeight; }

            // Measure text dimensions
            glm::vec2 measure(const char* s);

            void draw(
                    SpriteBatch& batch,
                    const char* s,
                    glm::vec2 position,
                    glm::vec2 scale,
                    float depth,
                    glm::vec4 tint,
                    Justification just = Justification::LEFT);
        private:
            static std::vector<int>* createRows(glm::ivec4* rects, int rectsLength, int r, int padding, int& w);

            int regStart, regLength;
            CharGlyph* glyphs;
            int fontHeight;

            unsigned int textureID;
    };
}

#endif // __SPRITEFONT__
