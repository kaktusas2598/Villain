#pragma once

#include "LinearMath/btIDebugDraw.h"
#include "rendering/DebugRenderer.hpp"

class BulletDebugRenderer: public btIDebugDraw {
    public:
        BulletDebugRenderer();
        virtual ~BulletDebugRenderer() {}
        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

        // NOTE: not yet implemented
        virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}
        virtual void reportErrorWarning(const char* warningString) {}
        virtual void draw3dText(const btVector3& location, const char* textString) {}

        // Render using Villain::DebugRenderer
        void render(const glm::mat4& mvpMatrix, float lineWidth);

        virtual void setDebugMode(int mode) { debugMode = mode; }
        virtual int getDebugMode() const { return debugMode; }
    private:
        Villain::DebugRenderer debugRenderer;
        int debugMode;
};
