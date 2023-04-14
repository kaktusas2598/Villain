#include "DebugRenderer.hpp"
#include <cmath>

namespace Villain {

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

    DebugRenderer ::DebugRenderer () { }

    DebugRenderer ::~DebugRenderer () {
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

    void DebugRenderer::init() {
        // Shader init
        debugShader = new Shader();
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

    void DebugRenderer::end() {
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

    glm::vec2 rotatePoint(glm::vec2 position, float angle) {
        glm::vec2 newPos;
        newPos.x = position.x * cos(angle) - position.y * sin(angle);
        newPos.y = position.x * sin(angle) + position.y * cos(angle);
        return newPos;
    }


    void DebugRenderer::drawBox(const glm::vec4& destRect, float depth, const glm::vec4& color, float angle) {
        int i = vertices.size(); // Index for 1st new vertex added
        vertices.resize(vertices.size() + 4);

        glm::vec2 halfDimensions(destRect.z / 2.0f, destRect.w / 2.0f);
        // Get points centered around origin
        glm::vec2 tl(-halfDimensions.x, halfDimensions.y); //top left
        glm::vec2 bl(-halfDimensions.x, -halfDimensions.y); //bottom left
        glm::vec2 br(halfDimensions.x, -halfDimensions.y); //bottom right
        glm::vec2 tr(halfDimensions.x, halfDimensions.y); //top right

        glm::vec2 positionOffset(destRect.x, destRect.y);

        vertices[i].position = glm::vec3(rotatePoint(tl, angle) + halfDimensions + positionOffset, depth);
        vertices[i + 1].position = glm::vec3(rotatePoint(bl, angle) + halfDimensions + positionOffset, depth);
        vertices[i + 2].position = glm::vec3(rotatePoint(br, angle) + halfDimensions + positionOffset, depth);
        vertices[i + 3].position = glm::vec3(rotatePoint(tr, angle) + halfDimensions + positionOffset, depth);

        for (int j = i; j < i + 4; j++) {
            vertices[j].color = color;
        }

        // Setup indices, we will use GL_LINES here
        indices.reserve(indices.size() + 8);

        indices.push_back(i);
        indices.push_back(i + 1);

        indices.push_back(i + 1);
        indices.push_back(i + 2);

        indices.push_back(i + 2);
        indices.push_back(i + 3);

        indices.push_back(i + 3);
        indices.push_back(i);
    }

    void DebugRenderer::drawCircle(const glm::vec2& center, float depth, const glm::vec4& color, float radius) {
        // Setup static for num of vertices in a circle
        static const int NUM_VERTS = 100;

        int start = vertices.size();
        vertices.resize(vertices.size() + NUM_VERTS);
        for (int i = 0; i < NUM_VERTS; i++) {
            float angle = ((float)i / NUM_VERTS) * 2.0f * M_PI;
            vertices[start + i].position.x = cos(angle) * radius + center.x;
            vertices[start + i].position.y = sin(angle) * radius + center.y;
            vertices[start + i].position.z = depth;
            vertices[start + i].color = color;
        }

        // Setup indices
        indices.reserve(indices.size() + (2 * NUM_VERTS));
        for (int i = 0; i < NUM_VERTS - 1; i++) {
            indices.push_back(start + i);
            indices.push_back(start + i + 1);
        }
        // Last 2 indices "loop around" and complete the circle
        indices.push_back(start + NUM_VERTS - 1);
        indices.push_back(start);
    }

    void DebugRenderer::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
        int i = vertices.size();
        vertices.resize(vertices.size() + 2);
        vertices[i].position = start;
        vertices[i].color = color;
        vertices[i + 0].position = end;
        vertices[i + 1].color = color;

        indices.reserve(indices.size() + 2);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    void DebugRenderer::drawBox3D(const glm::vec3& position, const glm::vec4& color, const glm::vec3& size) {
        int i = vertices.size(); // Index for 1st new vertex added
        vertices.resize(vertices.size() + 8);

        // position is center of the cube!!
        // 0 - FAR BOTTOM LEFT
        vertices[i].position.x = position.x - size.x/2;
        vertices[i].position.y = position.y - size.y/2;
        vertices[i].position.z = position.z - size.z/2;
        // 1 - FAR TOP LEFT
        vertices[i + 1].position.x = position.x - size.x/2;
        vertices[i + 1].position.y = position.y + size.y/2;
        vertices[i + 1].position.z = position.z - size.z/2;
        // 2 - FAR TOP RIGHT
        vertices[i + 2].position.x = position.x + size.x/2;
        vertices[i + 2].position.y = position.y + size.y/2;
        vertices[i + 2].position.z = position.z - size.z/2;
        // 3 - FAR BOTTOM RIGHT
        vertices[i + 3].position.x = position.x + size.x/2;
        vertices[i + 3].position.y = position.y - size.y/2;
        vertices[i + 3].position.z = position.z - size.z/2;
        // 4 - NEAR BOTTOM LEFT
        vertices[i + 4].position.x = position.x - size.x/2;
        vertices[i + 4].position.y = position.y - size.y/2;
        vertices[i + 4].position.z = position.z + size.z/2;
        // 5 - NEAR BOTTOM RIGHT
        vertices[i + 5].position.x = position.x + size.x/2;
        vertices[i + 5].position.y = position.y - size.y/2;
        vertices[i + 5].position.z = position.z + size.z/2;
        // 6 - NEAR TOP RIGHT
        vertices[i + 6].position.x = position.x + size.x/2;
        vertices[i + 6].position.y = position.y + size.y/2;
        vertices[i + 6].position.z = position.z + size.z/2;
        // 7 - NEAR TOP LEFT
        vertices[i + 7].position.x = position.x - size.x/2;
        vertices[i + 7].position.y = position.y + size.y/2;
        vertices[i + 7].position.z = position.z + size.z/2;

        // Set all vertex colors at once
        for (int j = i; j < i + 8; j++) {
            vertices[j].color = color;
        }

        indices.reserve(indices.size() + 24);

        // Far edges
        indices.push_back(i);
        indices.push_back(i + 1);

        indices.push_back(i + 1);
        indices.push_back(i + 2);

        indices.push_back(i + 2);
        indices.push_back(i + 3);

        indices.push_back(i + 3);
        indices.push_back(i);

        // Connecting edges
        indices.push_back(i);
        indices.push_back(i + 4);

        indices.push_back(i + 1);
        indices.push_back(i + 7);

        indices.push_back(i + 2);
        indices.push_back(i + 6);

        indices.push_back(i + 3);
        indices.push_back(i + 5);

        // Near edges
        indices.push_back(i + 4);
        indices.push_back(i + 7);

        indices.push_back(i + 7);
        indices.push_back(i + 6);

        indices.push_back(i + 6);
        indices.push_back(i + 5);

        indices.push_back(i + 5);
        indices.push_back(i + 4);
    }

    void DebugRenderer::drawBox3DRotated(const glm::vec3& position, const glm::vec3& size, const glm::mat4& rotation, const glm::vec4& color) {
        int i = vertices.size(); // Make sure to get latest vertex index before drawing axis aligned cube
        drawBox3D(position, color, size);

        // Revert vertices around origin (glm::vec3 position)
        for (int j = i; j < i + 8; j++) {
            // https://stackoverflow.com/questions/52770929/rotate-object-around-origin-as-it-faces-origin-in-opengl-with-glm
            glm::vec4 pos_h = rotation * glm::vec4(vertices[j].position - position, 1.0f);
            vertices[j].position = glm::vec3(pos_h) + position;
        }
    }

    // Inspired by: https://www.songho.ca/opengl/gl_sphere.html
    void DebugRenderer::drawSphere(const glm::vec3& center, float radius, const glm::vec4& color) {
        const float sectorCount = 36; //<<< num of longitude divisions
        const float stackCount = 18; //<<< num of latitude divisions
        float x, y, z, xy;

        int start = vertices.size(); // Index for 1st new vertex added
        vertices.resize(vertices.size() + 703);

        float sectorStep = 2 * M_PI / sectorCount;
        float stackStep = M_PI / stackCount;
        float sectorAngle, stackAngle;
        int vertexIndex = start;

        for (int i = 0; i <= stackCount; ++i) {
            stackAngle = M_PI / 2 - i * stackStep; //<<< from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectorCount; ++j) {
                sectorAngle = j * sectorStep;
                vertices[vertexIndex].position.x = xy * cosf(sectorAngle);
                vertices[vertexIndex].position.y = xy * sinf(sectorAngle);
                vertices[vertexIndex].position.z = z;
                // shift sphere by center vector to transform it
                vertices[vertexIndex].position += center;
                vertices[vertexIndex].color = color;
                vertexIndex++;
            }
        }
        // Generate indices
        indices.reserve(indices.size() + 2520);
        unsigned int k1, k2;
        for (int i = 0; i < stackCount; ++i) {
            k1 = i * (sectorCount + 1); // beginning of current stack
            k2 = k1 + sectorCount + 1; // beginning of next stack
            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                // vertical lines
                indices.push_back(start + k1);
                indices.push_back(start + k2);
                // horizontal lines
                if (i != 0) {
                    indices.push_back(start + k1);
                    indices.push_back(start + k1 + 1);
                }
            }
        }
    }

    void DebugRenderer::render(const glm::mat4& mvpMatrix, float lineWidth) {
        debugShader->bind();

        debugShader->setUniformMat4f("MVP", mvpMatrix);

        glLineWidth(lineWidth);

        glBindVertexArray(vao);
        glDrawElements(GL_LINES, numElements, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        debugShader->unbind();

    }
}
