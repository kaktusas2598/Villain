#include "VertexBuffer.hpp"

#include "Renderer.hpp"

VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum usage) {
    GLCall(glGenBuffers(1, &rendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
    if (size != -1)
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


void VertexBuffer::fill(const void* data, unsigned int size, GLenum usage) {
    bind();
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}
