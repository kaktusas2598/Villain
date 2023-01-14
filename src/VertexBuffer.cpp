#include "VertexBuffer.hpp"

#include "Renderer.hpp"

VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum usage) {
    GLCall(glGenBuffers(1, &rendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
    // TODO: when setting GL_DYNAMIC_DRAW, data needs to be nullptr so glBufferData needs
    // to know size and we will need a setter for data
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
}

VertexBuffer::~VertexBuffer() {
    GLCall(glDeleteBuffers(1, &rendererID));
}

void VertexBuffer::bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
}

void VertexBuffer::unbind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

