#ifndef __DEBUG_RENDERER__
#define __DEBUG_RENDERER__

#include <GL/glew.h>
#include "Shader.hpp"
#include "glm/glm.hpp"
#include <vector>

namespace Villain {

    class DebugRenderer {
        public:
            DebugRenderer ();
            ~DebugRenderer ();

            void init(); // Call in app initialisation method
            void end(); // Call before rendering
            void render(const glm::mat4& mvpMatrix, float lineWidth); // Will render all primitives using single batch

            // TODO:
            // 1. planes/rect (Not XY plane, but also XZ and YZ for 3D applications)
            // 2. Other 3D primitives (cylinders, different ways to calculate sphere?)
            // 3. Mesh generation could be refactored to new helper class?
            void drawBox(const glm::vec4& destRect, float depth, const glm::vec4& color, float angle);
            void drawCircle(const glm::vec2& center, float depth, const glm::vec4& color, float radius);
            void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f));
            void drawBox3D(const glm::vec3& position, const glm::vec4& color, const glm::vec3& size);
            void drawBox3DRotated(const glm::vec3& position, const glm::vec3& size, const glm::mat4& rotation, const glm::vec4& color = glm::vec4(1.0f));
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

#endif // __DEBUG_RENDERER__
