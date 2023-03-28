#ifndef __Camera2D__
#define __Camera2D__

#include "Camera.hpp"

namespace Villain {

    // Ortographic projection camera
    class Camera2D: public Camera {
        public:
            Camera2D();
            ~Camera2D();

            // Call after creation and on window size change
            void rescale(int width, int height);
            // At the moment "view" matrix for 2D camera is a combination of projection and view matrix and proj matrix here is simply identity matrix
            glm::mat4 getViewMatrix();
            glm::mat4 getProjMatrix();

            // TODO: need similar method for perspective 3D camera
            glm::vec2 screenToWorld(glm::vec2 screenCoords);

            // Checks if quad is in Camera's clip space
            bool quadInView(const glm::vec2& pos, const glm::vec2& dimensions);

            void offsetPosition(const glm::vec2& offset);
            void offsetScale(float offset);
    };
}
#endif // __Camera2D__
