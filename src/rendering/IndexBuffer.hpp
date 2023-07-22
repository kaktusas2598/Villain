#pragma once

#include "ErrorHandler.hpp"

namespace Villain {

    class IndexBuffer {
        public:
            IndexBuffer(const unsigned int* data, unsigned int cnt);
            ~IndexBuffer();

            void bind() const;
            void unbind() const;

            inline unsigned int getCount() const { return count; }
            void fill(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
        private:
            unsigned int rendererID;
            unsigned int count; //<<< Numner of indices
    };
}
