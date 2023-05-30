#ifndef __TERRAIN_FRUSTUM_CULLING__
#define __TERRAIN_FRUSTUM_CULLING__

#include "Frustum.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_access.hpp" // For accessing matrix rows and columns

// World space terrain frustum culling
// TODO: need to merge with Villain's Frustum class really
class FrustumCulling {
    public:
        FrustumCulling(const glm::mat4& viewProj) {
            update(viewProj);
        }

        void update(const glm::mat4& viewProj) {
            calculateClipPlanes(viewProj, leftClipPlane, rightClipPlane, topClipPlane, bottomClipPlane, nearClipPlane, farClipPlane);
        }

        bool isPointInsideViewFrustum(const glm::vec3& point) const {
            glm::vec4 p4D(point, 1.0f);

            bool inside =
                glm::dot(leftClipPlane, p4D) >= 0 &&
                glm::dot(rightClipPlane, p4D) <= 0 &&
                glm::dot(topClipPlane, p4D) <= 0 &&
                glm::dot(bottomClipPlane, p4D) >= 0 &&
                glm::dot(nearClipPlane, p4D) >= 0 &&
                glm::dot(farClipPlane, p4D) <= 0;

            return inside;
        }

    private:
        glm::vec4 leftClipPlane;
        glm::vec4 rightClipPlane;
        glm::vec4 topClipPlane;
        glm::vec4 bottomClipPlane;
        glm::vec4 nearClipPlane;
        glm::vec4 farClipPlane;

        void calculateClipPlanes(const glm::mat4& matrix, glm::vec4& left, glm::vec4& right, glm::vec4& top, glm::vec4& bottom, glm::vec4& near, glm::vec4& far) const {
            glm::vec4 row1 = glm::row(matrix, 0);
            glm::vec4 row2 = glm::row(matrix, 1);
            glm::vec4 row3 = glm::row(matrix, 2);
            glm::vec4 row4 = glm::row(matrix, 3);

            left = row1 + row4;
            right = row1 - row4;
            top = row2 - row4;
            bottom = row2 + row4;
            near = row3 + row4;
            far = row3 - row4;
        }
};

#endif // __TERRAIN_FRUSTUM_CULLING__
