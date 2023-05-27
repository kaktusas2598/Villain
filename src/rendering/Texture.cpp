#include "Texture.hpp"

#include "stb_image.h"

#include <iostream>

namespace Villain {

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

    Texture::Texture(const std::string& fileName, GLint wrappingMode, bool gammaCorrected)
        : rendererID(0), filePath(fileName), localBuffer(nullptr), width(0), height(0), BPP(4), target(GL_TEXTURE_2D) {

        stbi_set_flip_vertically_on_load(1);

        GLCall(glGenTextures(1, &rendererID));

        // NOTE: Pass nullptr instead of &BPP and set desired channels to 4 to ensure consistency
        // Alternatively 3rd argument can be &BPP and 4th argument null, so that we actually set same BPP as in file
        localBuffer = stbi_load(fileName.c_str(), &width, &height, nullptr, 4);

        if (localBuffer) {
            GLCall(glBindTexture(GL_TEXTURE_2D, rendererID));
            // Generate texture
            if (gammaCorrected) {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
            } else {
                GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
            }

            // TODO: support more mipmap types
            //GL_NEAREST_MIPMAP_NEAREST | GL_NEAREST_MIPMAP_LINEAR | GL_LINEAR_MIPMAP_NEAREST | GL_LINEAR_MIPMAP_LINEAR

            // Generate mipmap to remove artifacts on far away objects and reduce memory footprint
            // 2023-04-17: Should have done this way earlier as it increased performance in some examples
            // NOTE: must be done after glTexImage2D call
            GLCall(glGenerateMipmap(GL_TEXTURE_2D));
            // Setting anisotropy filter, best used with GL_LINEAR_MIPMAP_LINEAR, extension, but became core in OpenGL 4.6
            GLfloat maxAnisotropy;
            GLCall(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy)); // Query GPU for max available anisotropy
            GLCall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy));

            // Tell open GL how to filter texture when minifying or magnifying
            // how to wrap texture on x(s) and y(t) axis
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); // Mipmaping on magnification makes no sense
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode));

            GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        } else {
            std::cout << "Failed loading texture : " << fileName << std::endl;
            stbi_image_free(localBuffer);
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
