#ifndef __VertexBuffer__
#define __VertexBuffer__

#include "ErrorHandler.hpp"

class VertexBuffer {
    public:
        // If data is nullptr, then use fill() to call glBufferSubData()
        VertexBuffer(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;

        void fill(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
    private:
        unsigned int rendererID;
};

#endif // __VertexBuffer__
