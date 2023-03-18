#ifndef __Renderer__
#define __Renderer__

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"

namespace Villain {

    class Renderer {
        public:
            void clear() const;
            void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

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

#endif // __Renderer__
