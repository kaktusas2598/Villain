#pragma once

#include "LinearMath/btIDebugDraw.h"
#include "rendering/Shader.hpp"

// TODO: Main principle taken from Villain::DebugRenderer, which means it can be integrated
// with btIDebugDraw
class BulletDebugRenderer: public btIDebugDraw {
    public:
        BulletDebugRenderer();
        virtual ~BulletDebugRenderer();
        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

        void init(); // Call in app initialisation method
        void end(); // Call before rendering
        void render(const glm::mat4& mvpMatrix, float lineWidth); // Will render all primitives using single batch
        void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f));

        struct DebugVertex {
            glm::vec3 position;
            glm::vec4 color;
        };

        // NOTE: not implemented
        virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}
        virtual void reportErrorWarning(const char* warningString) {}
        virtual void draw3dText(const btVector3& location, const char* textString) {}

        virtual void setDebugMode(int mode) { debugMode = mode; }
        virtual int getDebugMode() const { return debugMode; }
    private:
        Villain::Shader* debugShader;
        std::vector<DebugVertex> vertices;
        std::vector<GLuint> indices;
        int numElements = 0;
        GLuint vbo = 0, vao = 0, ibo = 0;
        int debugMode;
};
