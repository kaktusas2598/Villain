#include "Texture.hpp"

#include "stb_image.h"

#include <iostream>

Texture::Texture(const std::string& fileName)
    : rendererID(0), filePath(fileName), localBuffer(nullptr), width(0), height(0), BPP(0), target(GL_TEXTURE_2D) {

    // FIXME:: For some reason flipping texture messes up tile atlases, but not spritesheets, super strange
    // Not sure why I need to flip texture for GL
    stbi_set_flip_vertically_on_load(1);

    localBuffer = stbi_load(fileName.c_str(), &width, &height, &BPP, 4);

    GLCall(glGenTextures(1, &rendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, rendererID));

    // Tell open GL how to filter texture when minifying or magnifying
    // how to wrap texture on x(s) and y(t) axis
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    // TODO: do I need mipmap stuff?

    // Generate texture
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    if (localBuffer)
        stbi_image_free(localBuffer);
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

