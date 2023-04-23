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
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));
        }

        GLCall(glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL));
    }

    Texture::Texture(const std::string& fileName, GLint wrappingMode)
        : rendererID(0), filePath(fileName), localBuffer(nullptr), width(0), height(0), BPP(0), target(GL_TEXTURE_2D) {

        stbi_set_flip_vertically_on_load(1);

        GLCall(glGenTextures(1, &rendererID));

        localBuffer = stbi_load(fileName.c_str(), &width, &height, &BPP, 4);

        if (localBuffer) {
            // NOTE: Does not seem to work fine, need to investigate this futher
            //GLenum format;
            //if (BPP == 1)
                //format = GL_RED;
            //else if (BPP == 3)
                //format = GL_RGB;
            //else if (BPP == 4)
                //format = GL_RGBA;
            //else
                //std::cout << "Unrecognized color format" << std::endl;

            GLCall(glBindTexture(GL_TEXTURE_2D, rendererID));
            // Generate texture
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
            // Generate mipmap to remove artifacts on far away objects and reduce memory footprint
            // 2023-04-17: Should have done this way earlier as it increased performance in some examples
            GLCall(glGenerateMipmap(GL_TEXTURE_2D));
            // Tell open GL how to filter texture when minifying or magnifying
            // how to wrap texture on x(s) and y(t) axis
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode));

            GLCall(glBindTexture(GL_TEXTURE_2D, 0));
            stbi_image_free(localBuffer);
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
    }

    void Texture::bind(unsigned int slot) const {
        GLCall(glActiveTexture(GL_TEXTURE0 + slot));
        GLCall(glBindTexture(target, rendererID));
    }

    void Texture::unbind() const {
        GLCall(glBindTexture(target, 0));
    }

}
