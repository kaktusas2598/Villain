#pragma once

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"

namespace Villain {

    // TODO: Possibly add beginScene() and addScene() methods and allow setting camera in the beginning
    // draw calls could probably take materials as well
    /*! \brief OpenGL rendering API abstraction
     *
     * Abstracts renderer drawing and clearing screen commands to avoid raw OpenGL calls
     */
    class Renderer {
        public:
            /// Clear the screen
            void clear() const;
            /// Set color for screen clear
            void setClearColor(const glm::vec4& color);
            /// Draw instanced (optionally) indexed mesh using given shader
            void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned instances = 1) const;

            /// Get singleton instance
            static Renderer *Instance() {
                if (sInstance == 0)
                {
                    sInstance = new Renderer();
                    return sInstance;
                }
                return sInstance;
            }

        private:
            static Renderer* sInstance;
    };
}
