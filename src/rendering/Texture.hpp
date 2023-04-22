#ifndef __Texture__
#define __Texture__

#include "ErrorHandler.hpp"
#include <vector>

namespace Villain  {

    class Texture {
        public:
            // 2D Texture
            Texture(const std::string& fileName, GLint wrappingMode = GL_CLAMP_TO_EDGE);
            // Cubemap Texture
            Texture(std::vector<std::string> faces);
            ~Texture();

            void bind(unsigned int slot = 0) const;
            void unbind() const;

            inline int getWidth() const { return width; }
            inline int getHeight() const { return height; }

            void setType(std::string& t) { type = t; }
            inline std::string getType() const { return type; }

            unsigned int getID() { return rendererID; }
        private:
            unsigned int rendererID;
            std::string filePath;
            unsigned char* localBuffer;
            int width, height, BPP; // Bits per picture
            // Available types: texture_diffuse, texture_specular, texture_normal
            std::string type;
            GLenum target; ///< GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP
            // For when we want to bind multiple textures to same framebuffer
            //GLuint* renderIDs
    };
}

#endif // __Texture__
