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

            void init();
            void end();
            void drawBox(const glm::vec4& destRect, float depth, const glm::vec4& color, float angle);
            void drawCircle(const glm::vec2& center, float depth, const glm::vec4& color, float radius);
            // Will render all as one batch
            void render(const glm::mat4& mvpMatrix, float lineWidth);
            void dispose();

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