#ifndef __VertexBufferLayout__
#define __VertexBufferLayout__

#include <vector>
#include "Renderer.hpp"

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalised;

    static unsigned int getSizeOfType(unsigned int type) {
        switch(type) {
            case GL_FLOAT:              return 4;
            case GL_UNSIGNED_INT:       return 4;
            case GL_UNSIGNED_BYTE:      return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout {
    public:
        VertexBufferLayout(): stride(0) {
        }
        ~VertexBufferLayout() {
        }

        template <typename T> void push (unsigned int count) {
            //static_assert(false);
        }


        inline const std::vector<VertexBufferElement> getElements() const { return elements; }
        inline unsigned int getStride() const { return stride; }

    private:
        std::vector<VertexBufferElement> elements;
        unsigned int stride;
};

template<> inline void VertexBufferLayout::push<float>(unsigned int count) {
    elements.push_back({GL_FLOAT, count, GL_FALSE});
    stride += VertexBufferElement::getSizeOfType(GL_FLOAT) * count;
}

template<> inline void VertexBufferLayout::push<unsigned int>(unsigned int count) {
    elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT) * count;
}

template<> inline void VertexBufferLayout::push<unsigned char>(unsigned int count) {
    elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE) * count;
}


#endif // __VertexBufferLayout__
