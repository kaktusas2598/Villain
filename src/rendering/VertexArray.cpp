#include "VertexArray.hpp"

#include "VertexBufferLayout.hpp"
#include "Renderer.hpp"

namespace Villain {

    VertexArray::VertexArray() {
        // Using vertex array means we dont need to specigy vertex attributes every time we draw
        // also let's us specify different vertex layouts, default vao can be used with compability profile
        // core profile requires vao to be set
        GLCall(glGenVertexArrays(1, &rendererID));
    }

    VertexArray::~VertexArray() {
        GLCall(glDeleteVertexArrays(1, &rendererID));
    }

    void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
        bind();
        // First bind vertex buffer of course
        vb.bind();

        // Specifying vertex layout below by enabling and configuring vertex vattributes
        const auto& elements = layout.getElements();
        unsigned int offset = 0;
        for (unsigned int i = 0; i < elements.size(); i++) {
            const auto& element = elements[i];
            // Enable vertex attributes
            GLCall(glEnableVertexAttribArray(i));
            // Set up vertex attributes (position, colour, texture uv, normals)
            // FIXME: Had to hardcode GL_INT instead of element.type here as OpenGL throws error then, WTF?? makes no sense whatsoever
            // GL_INVALID_ENUM is generated if type is not an accepted value.
            if (element.type == GL_INT) {
                //GLCall(glVertexAttribIPointer(i, element.count, GL_INT, layout.getStride(), (const void*)offset));
                GLCall(glVertexAttribIPointer(i, 4, GL_INT, layout.getStride(), reinterpret_cast<const void*>(offset)));
            } else {
                GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.getStride(), reinterpret_cast<const void*>(offset)));
            }
            offset += element.count * VertexBufferElement::getSizeOfType(element.type);
        }
    }

    void VertexArray::bind() const {
        GLCall(glBindVertexArray(rendererID));
    }

    void VertexArray::unbind() const {
        GLCall(glBindVertexArray(0));
    }
}
