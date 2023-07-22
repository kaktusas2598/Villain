#pragma once

#include <GL/glew.h>
#include "Shader.hpp"
#include "glm/glm.hpp"
#include <vector>

namespace Villain {

    /// Render primitives for debug purposes using lines
    // TODO: Plane grid (from plane normal and distance to origin)
    // TODO: Other 3D primitives (cylinders, different ways to calculate sphere?)
    // TODO: Mesh generation could be refactored to new helper class?
    class DebugRenderer {
        public:
            ~DebugRenderer();

            /// Call in app initialisation method, must be called before doing anything else
            void init();
            // Call before calling render
            void end();
            /// Renders all primitives using single batch
            void render(const glm::mat4& mvpMatrix, float lineWidth);


            /// Add 2D Box (on XY plane) to draw batch
            void drawBox(const glm::vec4& destRect, float depth, const glm::vec4& color, float angle);
            /// Add Circle (on XY plane) to draw batch
            void drawCircle(const glm::vec2& center, float depth, const glm::vec4& color, float radius);
            /// Add line segment to draw batch
            void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f));
            /// Add axis-aligned quad to draw batch
            void drawQuad(const glm::vec3& center, glm::vec2& size, bool x, bool y, bool z, const glm::vec4& color = glm::vec4(1.0f));
            /// Add axis-aligned box (AABB) to draw batch
            void drawBox3D(const glm::vec3& position, const glm::vec4& color, const glm::vec3& size);
            /// Add rotated box (OBB) to draw batch
            void drawBox3DRotated(const glm::vec3& position, const glm::vec3& size, const glm::mat4& rotation, const glm::vec4& color = glm::vec4(1.0f));
            /// Add sphere to draw batch
            void drawSphere(const glm::vec3& center, float radius, const glm::vec4& color = glm::vec4(1.0f));

            struct DebugVertex {
                glm::vec3 position;
                glm::vec4 color;
            };
        private:
            Shader* debugShader;
            std::vector<DebugVertex> vertices;
            std::vector<GLuint> indices;
            int numElements = 0;
            GLuint vbo = 0, vao = 0, ibo = 0;
    };
}
