#pragma once

#include "VertexBuffer.hpp"

namespace Villain {

    class VertexBufferLayout;

    class VertexArray {
        public:
            VertexArray();
            ~VertexArray();

            void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

            void bind() const;
            void unbind() const;

        private:
            unsigned int rendererID;

    };
}
