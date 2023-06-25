#include <catch2/catch.hpp>

#include "rendering/IndexBuffer.hpp"

#include <catch2/catch.hpp>
#include "trompeloeil/include/catch2/trompeloeil.hpp"

class MockOpenGL {
public:
    MAKE_MOCK2(glGenBuffers, void(GLsizei, GLuint*));
    MAKE_MOCK2(glBindBuffer, void(GLenum, GLuint));
    MAKE_MOCK4(glBufferData, void(GLenum, GLsizeiptr, const void*, GLenum));
    MAKE_MOCK4(glBufferSubData, void(GLenum, GLintptr, GLsizeiptr, const void*));
    MAKE_MOCK2(glDeleteBuffers, void(GLsizei, const GLuint*));

    virtual ~MockOpenGL() = default;  // Add virtual destructor
};

trompeloeil::deathwatched<MockOpenGL> mockOpenGL;


using trompeloeil::_;

TEST_CASE("IndexBuffer bind") {
    IndexBuffer buffer(nullptr, 0);
    REQUIRE_CALL(mockOpenGL, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _)).TIMES(1);
    buffer.bind();
}

TEST_CASE("IndexBuffer unbind") {
    IndexBuffer buffer(nullptr, 0);
    REQUIRE_CALL(mockOpenGL, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)).TIMES(1);
    buffer.unbind();
}

TEST_CASE("IndexBuffer fill") {
    IndexBuffer buffer(nullptr, 0);
    const void* data = nullptr;
    unsigned int size = 0;
    GLenum usage = GL_STATIC_DRAW;
    REQUIRE_CALL(mockOpenGL, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _)).TIMES(1);
    REQUIRE_CALL(mockOpenGL, glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage)).TIMES(1);
    REQUIRE_CALL(mockOpenGL, glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data)).TIMES(1);
    buffer.fill(data, size, usage);
}
