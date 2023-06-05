#include "Renderer.hpp"
#include "ErrorHandler.hpp"

#include <iostream>

namespace Villain {

    Renderer* Renderer::sInstance = nullptr;

    void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned instances) const {
        shader.bind();

        // By using vertex array obbject, we dont need to bind array buffer and vertex attributes 2nd time
        va.bind();
        ib.bind();

        if (instances > 1) {
            GLCall(glDrawElementsInstanced(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr, instances));
        } else {
            GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
        }
    }

    void Renderer::clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}
