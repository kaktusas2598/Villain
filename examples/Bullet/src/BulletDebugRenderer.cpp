#include "BulletDebugRenderer.hpp"

void BulletDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    debugRenderer.drawLine(
            glm::vec3(from.getX(), from.getY(), from.getZ()),
            glm::vec3(to.getX(), to.getY(), to.getZ()),
            glm::vec4(color.getX(), color.getY(), color.getZ(), 1.0f));
}

BulletDebugRenderer::BulletDebugRenderer(): debugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb) {
    debugRenderer.init();
}

void BulletDebugRenderer::render(const glm::mat4& mvpMatrix, float lineWidth) {
    debugRenderer.end();
    debugRenderer.render(mvpMatrix, lineWidth);
}
