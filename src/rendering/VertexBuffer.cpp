#include "VertexBuffer.hpp"

#include "Renderer.hpp"

namespace Villain {

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
        // NOTE: should be safe not to have this as glBufferData allocates and coppies data to vertex buffer
        // and sub data just copies verties to the same data
        // GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
    }
}
