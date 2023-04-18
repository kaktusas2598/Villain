#include "IndexBuffer.hpp"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int cnt)
    : count(cnt) {
    GLCall(glGenBuffers(1, &rendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID));
    // Keep in mind there is a slim chance unsigned int on some platforms will not be 4 bytes, in that case use GLUint
    if (cnt != -1)
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer() {
    GLCall(glDeleteBuffers(1, &rendererID));
}

void IndexBuffer::bind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID));
}

void IndexBuffer::unbind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::fill(const void* data, unsigned int size, GLenum usage) {
    bind();
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage));
    GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data));
}
