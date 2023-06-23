#include "Texture.hpp"

#include "stb_image.h"

#include <iostream>

namespace Villain {

    Texture::Texture(const std::string& fileName, TextureConstructionInfo texInfo) :
        rendererID(0), filePath(fileName), localBuffer(texInfo.DataBuffer),
        width(texInfo.Width), height(texInfo.Height), BPP(texInfo.BPP), target(texInfo.Target)
    {
        stbi_set_flip_vertically_on_load(1);

        if (rendererID == 0) {
            GLCall(glGenTextures(1, &rendererID));
        }

        // First handle loading file texture into buffer if filename was set
        if (!filePath.empty()) {
            // NOTE: Pass nullptr instead of &BPP and set desired channels to 4 to ensure consistency
            // Alternatively 3rd argument can be &BPP and 4th argument null, so that we actually set same BPP as in file
            std::cout << "Loading texture: " << filePath.c_str() << ". Gamma corrected: " << texInfo.SRGB << "\n";
            localBuffer = stbi_load(filePath.c_str(), &width, &height, nullptr, 4);
            BPP = 4;

            if (!localBuffer) {
                std::cout << "Failed loading texture : " << filePath << std::endl;
                stbi_image_free(localBuffer);
                return;
            }
        }

        // Recalibrate for gamma correction
        if (texInfo.SRGB) {
            if (BPP == 4)
                texInfo.InternalFormat = GL_SRGB_ALPHA;
            else
                texInfo.InternalFormat = GL_SRGB;
        }

        // NOTE: Not neccessary for now, as stb loaded textures are hardcoded to 4 BPP,
        // otherwise we will set our own formats using TextureConstructionInfo struct
        //GLenum format;
        //if (BPP == 1) {
            //texInfo.InternalFormat = texInfo.SRGB ? GL_SRGB: GL_RED;
            //texInfo.Format = GL_RED;
        //} else if (BPP == 3) {
            //texInfo.InternalFormat = texInfo.SRGB ? GL_SRGB: GL_RGB;
            //texInfo.Format = GL_RGB;
        //} else if (BPP == 4) {
            //texInfo.InternalFormat = texInfo.SRGB ? GL_SRGB_ALPHA : GL_RGBA;
            //texInfo.Format = GL_RGBA;
        //} else
            //std::cout << "Unrecognized color format" << std::endl;


        GLCall(glBindTexture(target, rendererID));
        GLCall(glTexImage2D(target, 0, texInfo.InternalFormat, width, height, 0, texInfo.Format, texInfo.DataType, localBuffer));

        if (texInfo.GenerateMipmap) {
            // TODO: support more mipmap types
            //GL_NEAREST_MIPMAP_NEAREST | GL_NEAREST_MIPMAP_LINEAR | GL_LINEAR_MIPMAP_NEAREST | GL_LINEAR_MIPMAP_LINEAR
            GLCall(glGenerateMipmap(target));
            // Setting anisotropy filter, best used with GL_LINEAR_MIPMAP_LINEAR, extension, but became core in OpenGL 4.6
            GLfloat maxAnisotropy;
            GLCall(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy)); // Query GPU for max available anisotropy
            GLCall(glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy));

            // Tell open GL how to filter texture when minifying or magnifying
            // how to wrap texture on x(s) and y(t) axis
            GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); // Mipmaping on magnification makes no sense
        } else {
            GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texInfo.Filter));
            GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, texInfo.Filter));
        }

        if (texInfo.Clamp) {
            GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, texInfo.WrappingMode));
            GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, texInfo.WrappingMode));
        }

        GLCall(glBindTexture(target, 0));
    }

    Texture::Texture(uint32_t bufferSize, void* buffer, bool sRGB): target(GL_TEXTURE_2D) {
        void * imageData = stbi_load_from_memory((const stbi_uc*)buffer, bufferSize, &width, &height, &BPP, 0);

        GLCall(glGenTextures(1, &rendererID));
        GLCall(glBindTexture(target, rendererID));

        // Recalibrate for gamma correction
        //GLenum internalFormat;
        //if (sRGB) {
            //if (BPP == 4)
                //internalFormat = GL_SRGB_ALPHA;
            //else
                //internalFormat= GL_SRGB;
        //}
        GLCall(glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData));

        GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameterf(target, GL_TEXTURE_BASE_LEVEL, 0));
        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT));

        GLCall(glGenerateMipmap(target));

        GLCall(glBindTexture(target, 0));


        stbi_image_free(imageData);

    }

    void Texture::init(int w, int h, unsigned int id, GLfloat filter, GLint internalFormat, GLenum format, bool clamp) {
        rendererID = id;
        width = w;
        height = h;
        if (rendererID == 0) {
            GLCall(glGenTextures(1, &rendererID));
        }
        GLCall(glBindTexture(target, rendererID));
        GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter));
        GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter));

        if (clamp) {
            GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
            GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
            // Set border color for shadow mapping as white so we don't get duplicate shadows
            float borderColour[] = {1.0, 1.0, 1.0, 1.0};
            GLCall(glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColour));
        }

        GLCall(glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_FLOAT, NULL));
    }

    void Texture::init(int w, int h, int bpp, unsigned char* textureData) {
        width = w;
        height = h;
        BPP = bpp;

        GLenum format;
        if (BPP == 1)
            format = GL_RED;
        else if (BPP == 3)
            format = GL_RGB;
        else if (BPP == 4)
            format = GL_RGBA;
        else
            std::cout << "Unrecognized color format" << std::endl;

        GLCall(glGenTextures(1, &rendererID));
        GLCall(glBindTexture(target, rendererID));
        GLCall(glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData));

        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
        GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT));
    }

    void Texture::initCubeMap(int w, int h, unsigned int id, GLfloat filter, GLint internalFormat, GLenum format) {
        rendererID = id;
        width = w;
        height = h;
        if (rendererID == 0) {
            GLCall(glGenTextures(1, &rendererID));
        }
        GLCall(glBindTexture(target, rendererID));
        for (unsigned int i = 0; i < 6; ++i) {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_FLOAT, NULL));
        }

        GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter));
        GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter));

        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    }

    Texture::Texture(std::vector<std::string> faces)
        : rendererID(0), localBuffer(nullptr), width(0), height(0), BPP(0), target(GL_TEXTURE_CUBE_MAP) {
        stbi_set_flip_vertically_on_load(0);

        GLCall(glGenTextures(1, &rendererID));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID));

        for (unsigned int i = 0; i < faces.size(); i++) {
            localBuffer = stbi_load(faces[i].c_str(), &width, &height, &BPP, 0);

            if (localBuffer) {
                GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                            GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, localBuffer));
                stbi_image_free(localBuffer);
            } else {
                std::cout << "Failed loading cubemap texture : " << faces[i] << std::endl;
            }
        }

        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

        GLCall(glBindTexture(GL_TEXTURE_2D, 0));

        stbi_set_flip_vertically_on_load(1);
    }

    Texture::~Texture() {
        std::cout << "Deleting texture: " << filePath << std::endl;
        GLCall(glDeleteTextures(1, &rendererID));
        if (localBuffer)
            stbi_image_free(localBuffer);
    }

    void Texture::bind(unsigned int slot) const {
        GLCall(glActiveTexture(GL_TEXTURE0 + slot));
        GLCall(glBindTexture(target, rendererID));
    }

    void Texture::unbind() const {
        GLCall(glBindTexture(target, 0));
    }

    glm::vec3 Texture::getColor(int x, int y) {
        int wrappedX = x % width;
        int wrappedY = y % height;

        unsigned char* pixelPointer = localBuffer + (wrappedX + wrappedY * width) * BPP;

        glm::vec3 color;
        color.r = (float)pixelPointer[0];
        color.g = (float)pixelPointer[1];
        color.b = (float)pixelPointer[2];

        return color;
    }
}
