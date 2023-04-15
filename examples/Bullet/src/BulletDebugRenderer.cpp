#include "BulletDebugRenderer.hpp"
#include <cmath>

const char* DEBUG_VERTEX_SRC = R"(
    #version 330 core

    layout(location = 0) in vec3 position;
    layout(location = 1) in vec4 color;

    out vec3 v_position;
    out vec4 v_color;

    uniform mat4 MVP;

    void main() {
        gl_Position = MVP * vec4(position, 1.0);
        v_color = color;
        v_position = position;
    }
    )";

const char* DEBUG_FRAGMENT_SRC = R"(
    #version 330 core

    layout (location = 0) out vec4 fragColor;

    in vec4 v_color;
    in vec3 v_position;

    void main() {
        // Transparency HACK
        if (v_color.a < 0.5) discard; else fragColor = v_color;
    }
    )";


void BulletDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    // TODO:
    drawLine(
            glm::vec3(from.getX(), from.getY(), from.getZ()),
            glm::vec3(to.getX(), to.getY(), to.getZ()),
            glm::vec4(color.getX(), color.getY(), color.getZ(), 1.0f));
}

BulletDebugRenderer::BulletDebugRenderer(): debugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb) {}

BulletDebugRenderer::~BulletDebugRenderer() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    if (ibo) {
        glDeleteBuffers(1, &ibo);
    }
}

void BulletDebugRenderer::init() {
    // Shader init
    debugShader = new Villain::Shader();
    debugShader->createFromSource(std::string(DEBUG_VERTEX_SRC), std::string(DEBUG_FRAGMENT_SRC));

    // Gen buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Setup vertex attributes
    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, position));
    glEnableVertexAttribArray(1); // Colour
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));

    glBindVertexArray(0);
}

void BulletDebugRenderer::end() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // orphan the buffer
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
    // upload the data to the buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(DebugVertex), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // orphan the buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
    // upload the data to the buffer
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    numElements = indices.size();
    indices.clear();
    vertices.clear();
}

void BulletDebugRenderer::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
    int i = vertices.size();
    vertices.resize(vertices.size() + 2);
    vertices[i].position = start;
    vertices[i].color = color;
    vertices[i + 1].position = end;
    vertices[i + 1].color = color;

    indices.reserve(indices.size() + 2);
    indices.push_back(i);
    indices.push_back(i + 1);
}


void BulletDebugRenderer::render(const glm::mat4& mvpMatrix, float lineWidth) {
    debugShader->bind();

    debugShader->setUniformMat4f("MVP", mvpMatrix);

    glLineWidth(lineWidth);

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, numElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    debugShader->unbind();
}
