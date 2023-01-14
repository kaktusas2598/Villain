#include "Renderer.hpp"

#include <iostream>

void GLClearError() {
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* func, const char* file, int line) {
    while(GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] " << "(" << error << "): " << func << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
    shader.bind();
    //shader.setUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

    // By using vertex array obbject, we dont need to bind array buffer and vertex attributes 2nd time
    va.bind();
    ib.bind();

    // Needs index buffer object, but this way we save memory and vertex data is smaller
    GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));

}

void Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT);
}
