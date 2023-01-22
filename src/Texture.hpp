#ifndef __Texture__
#define __Texture__

#include "Renderer.hpp"
#include <vector>

class Texture {
    public:
        // 2D Texture
        Texture(const std::string& fileName);
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
        std::string type;
        GLenum target; ///< GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP
};

#endif // __Texture__
