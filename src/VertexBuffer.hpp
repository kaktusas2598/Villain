#ifndef __VertexBuffer__
#define __VertexBuffer__

#include "ErrorHandler.hpp"

class VertexBuffer {
    public:
        VertexBuffer(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;
    private:
        unsigned int rendererID;
};

#endif // __VertexBuffer__
