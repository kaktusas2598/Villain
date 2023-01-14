#ifndef __VertexArray__
#define __VertexArray__

#include "VertexBuffer.hpp"

class VertexBufferLayout;

class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

        void bind() const;
        void unbind() const;

    private:
        unsigned int rendererID;

};

#endif // __VertexArray__
