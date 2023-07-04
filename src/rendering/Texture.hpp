#pragma once

#include "ErrorHandler.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace Villain  {

    enum class TextureMapType {
        NONE, DIFFUSE, SPECULAR, NORMAL, HEIGHT, DISPLACEMENT
    };

    // OpenGL texture parameters info for easier texture construction
    // Defaults to RGBA 2D texture
    struct TextureConstructionInfo {
        GLenum Target = GL_TEXTURE_2D; //<<< Can also be GL_TEXTURE_CUBE_MAP
        GLfloat Filter = GL_LINEAR; //<<< Can also be GL_NEAREST
        // Other options: GL_RGB, GL_RED, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16 and others
        GLint InternalFormat = GL_RGBA;
        GLenum Format = GL_RGBA;
        GLenum DataType = GL_UNSIGNED_BYTE; //<<< Can also be GL_FLOAT

        bool Clamp = true;
        GLint WrappingMode = GL_CLAMP_TO_EDGE;

        unsigned char* DataBuffer = nullptr;
        int Width = 0, Height = 0, BPP = 4;
        bool GenerateMipmap = true;
        bool SRGB = true;
        TextureMapType MapType = TextureMapType::DIFFUSE;

        //static TextureConstructionInfo depth2DBuffer();
        static TextureConstructionInfo customTexture(unsigned char* buffer, int BPP = 4) {
            TextureConstructionInfo customTextureInfo = TextureConstructionInfo();
            customTextureInfo.DataBuffer = buffer;
            customTextureInfo.BPP = BPP;
            return customTextureInfo;
        };
    };

    class Texture {
        public:
            Texture(const std::string& fileName = std::string(), TextureConstructionInfo texInfo = TextureConstructionInfo());
            Texture(uint32_t bufferSize, void* buffer, bool sRGB = true); // For embedded model textures

            // Initialise empty texture, used by framebuffers
            Texture(GLenum targetType = GL_TEXTURE_2D) : rendererID(0), target(targetType) {}
            void init(int w, int h, unsigned int id = 0, GLfloat filter = GL_LINEAR, GLint internalFormat = GL_RGBA, GLenum format = GL_RGBA, bool clamp = false);
            void init(int w, int h, int bpp, unsigned char* textureData);

            // Created specifically to init depth cubemap used for omnidirectional shadow mapping
            void initCubeMap(int w, int h, unsigned int id = 0, GLfloat filter = GL_NEAREST, GLint internalFormat = GL_DEPTH_COMPONENT16, GLenum format = GL_DEPTH_COMPONENT);

            // 2D Texture
            //Texture(const std::string& fileName, GLint wrappingMode = GL_CLAMP_TO_EDGE, bool gammaCorrected = true);

            // Cubemap Texture
            Texture(std::vector<std::string> faces);

            ~Texture();

            void bind(unsigned int slot = 0) const;
            void unbind() const;

            inline int getWidth() const { return width; }
            inline int getHeight() const { return height; }
            glm::vec3 getColor(int x, int y);

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
    };
}
