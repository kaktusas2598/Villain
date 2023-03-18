#include "Renderer.hpp"
#include "ErrorHandler.hpp"

#include <iostream>

namespace Villain {

    Renderer* Renderer::sInstance = nullptr;

    void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
        shader.bind();

        // By using vertex array obbject, we dont need to bind array buffer and vertex attributes 2nd time
        va.bind();
        ib.bind();

        // Needs index buffer object, but this way we save memory and vertex data is smaller
        GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
    }

    void Renderer::clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}
