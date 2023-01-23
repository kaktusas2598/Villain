#ifndef __Camera2D__
#define __Camera2D__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Villain {

    class Camera2D {
        public:
            Camera2D();
            ~Camera2D();

            // Call after creation and on window size change
            void init(int width, int height);
            void update();
            // Combination of projection and view matrix
            glm::mat4 getCameraMatrix();

            // Setters
            void setPosition(const glm::vec3& newPos) { position = newPos; }
            void setZoom(float z) { zoom = z; }
            // Getters
            glm::vec3 getPosition() const { return position; }
            float getZoom() const { return zoom; }

        private:
            int screenWidth, screenHeight;
            float zoom;
            glm::vec3 position;
            glm::mat4 projection;
    };
}
#endif // __Camera2D__