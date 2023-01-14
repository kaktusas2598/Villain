#ifndef __Renderer__
#define __Renderer__

#include <GL/glew.h>

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"


// In Vigilant, these Macros below will only, if at all, will be used for debug builds only
//#ifdef DEBUG
    //#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(...))
//#else
    //#define GLCall(x) x
//#endif
//Also these macros would go to error handler classes

// raise() is POSIX system specific
// could not use _debugbreak() because it's MVSC specific
#include <signal.h>
#define ASSERT(x) if(!(x)) raise(SIGABRT)
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* func, const char* file, int line);


class Renderer {
    public:
        void clear() const;
        void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    private:
};

#endif // __Renderer__
