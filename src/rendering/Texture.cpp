#include "Texture.hpp"

#include "Logger.hpp"
#include "stb_image.h"

namespace Villain {

    Texture::Texture(const std::string& fileName, TextureConstructionInfo texInfo) :
        rendererID(0), filePath(fileName), localBuffer(texInfo.DataBuffer),
        width(texInfo.Width), height(texInfo.Height), BPP(texInfo.BPP), target(texInfo.Target)
    {
        GLCall(glGenTextures(1, &rendererID));

        // First handle loading file texture into buffer if filename was set
        if (!filePath.empty()) {
            stbi_set_flip_vertically_on_load(1);
            // NOTE: Pass nullptr instead of &BPP and set desired channels to 4 to ensure consistency
            // Alternatively 3rd argument can be &BPP and 4th argument null, so that we actually set same BPP as in file
            Logger::Instance()->info("Loading texture: {} Gamma corrected: {}", filePath, texInfo.SRGB);
            localBuffer = stbi_load(filePath.c_str(), &width, &height, &BPP, 0);

            if (!localBuffer) {
                VILLAIN_ERROR("Failed loading texture: {}", filePath);
                stbi_image_free(localBuffer);
                return;
            }
        }

        // Recalibrate for gamma correction
        if (BPP == 4)
            texInfo.InternalFormat = texInfo.SRGB ? GL_SRGB_ALPHA : GL_RGBA;
        else if (BPP == 3) {
            texInfo.InternalFormat = texInfo.SRGB ? GL_SRGB: GL_RGB;
            texInfo.Format = GL_RGB;
        } else if (BPP == 1)
            texInfo.InternalFormat = texInfo.Format = GL_RED;
        else
            VILLAIN_ERROR("Unsupported number of channels: {}", BPP);

        GLCall(glBindTexture(target, rendererID));
        GLCall(glTexImage2D(target, 0, texInfo.InternalFormat, width, height, 0, texInfo.Format, texInfo.DataType, localBuffer));

        if (texInfo.GenerateMipmap) {
            setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
        } else {
            setFiltering(texInfo.Filter, texInfo.Filter);
        }

        if (texInfo.Clamp) {
            setWrapping(texInfo.WrappingMode);
        }

        GLCall(glBindTexture(target, 0));
    }

    Texture::Texture(uint32_t bufferSize, void* buffer, bool sRGB): target(GL_TEXTURE_2D) {
        stbi_set_flip_vertically_on_load(1);
        void * imageData = stbi_load_from_memory((const stbi_uc*)buffer, bufferSize, &width, &height, &BPP, 0);

        GLCall(glGenTextures(1, &rendererID));
        GLCall(glBindTexture(target, rendererID));

        GLint internalFormat;
        GLenum format;
        if (BPP == 4) {
            internalFormat = sRGB ? GL_SRGB_ALPHA : GL_RGBA;
            format = GL_RGBA;
        } else {
            internalFormat = sRGB ? GL_SRGB : GL_RGB;
            format = GL_RGB;
        }

        GLCall(glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, imageData));

        setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        setWrapping(GL_REPEAT);

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
        setFiltering(filter, filter);

        if (clamp) {
            setWrapping(GL_CLAMP_TO_BORDER);
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
            Logger::Instance()->error("Unrecognized colour format: {}", format);

        GLCall(glGenTextures(1, &rendererID));
        GLCall(glBindTexture(target, rendererID));
        GLCall(glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData));

        setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        setWrapping(GL_REPEAT);
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

        setFiltering(filter, filter);
        setWrapping(GL_CLAMP_TO_EDGE);
    }

    void Texture::loadHDR(const std::string& fileName) {
        stbi_set_flip_vertically_on_load(true);
        float *data = stbi_loadf(fileName.c_str(), &width, &height, &BPP, 0);
        if (data) {
            glGenTextures(1, &rendererID);
            glBindTexture(GL_TEXTURE_2D, rendererID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

            setWrapping(GL_CLAMP_TO_EDGE);
            setFiltering(GL_LINEAR, GL_LINEAR);

            stbi_image_free(data);
        }
        else {
            VILLAIN_ERROR("Failed loading texture: {}", filePath);
        }
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
                Logger::Instance()->error("Failed loading cubemap texture : {}", faces[i]);
            }
        }

        setFiltering(GL_LINEAR, GL_LINEAR);
        setWrapping(GL_CLAMP_TO_EDGE);

        GLCall(glBindTexture(GL_TEXTURE_2D, 0));

        stbi_set_flip_vertically_on_load(1);
    }

    Texture::~Texture() {
        VILLAIN_TRACE("Deleting texture: {}", filePath);
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

    void Texture::setFiltering(GLenum minFilter, GLenum magFilter, bool anisotropy) {
        // Texture must be bound before calling this! But binding is expensive so no call included here
        GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter));
        GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter));

        if (minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST ||
                minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_NEAREST_MIPMAP_NEAREST) {
            glGenerateMipmap(target);
            if (anisotropy) {
                GLfloat maxAnisotropy;
                GLCall(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy)); // Query GPU for max available anisotropy
                GLCall(glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy));
            }

        }
    }

    void Texture::setWrapping(GLenum wrappingMode) {
        // Texture must be bound before calling this! But binding is expensive so no call included here
        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, wrappingMode));
        GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, wrappingMode));
        if (target == GL_TEXTURE_CUBE_MAP)
            GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_R, wrappingMode));
    }
}
