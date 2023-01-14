#include "Texture.hpp"

#include "stb_image.h"

#include <iostream>

Texture::Texture(const std::string& fileName)
    : rendererID(0), filePath(fileName), localBuffer(nullptr), width(0), height(0), BPP(0) {

    // Not sure why I need to flip texture for GL
    stbi_set_flip_vertically_on_load(1);

    std::cout << "Loading texture: " << fileName.c_str() << std::endl;
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

Texture::~Texture() {
    std::cout << "Deleting texture: " << filePath << std::endl;
    GLCall(glDeleteTextures(1, &rendererID));
}

void Texture::bind(unsigned int slot) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, rendererID));
}

void Texture::unbind() const {
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

