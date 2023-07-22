#pragma once

namespace Villain {

    class Camera;

    enum class CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Utilise strategy pattern to allow camera to select different type of projection and control algorithm
    class CameraController {
        public:
            CameraController(Camera* cam): camera(cam) {}

            // NOTE: so far only 1 method is enough here
            virtual void update(float deltaTime) = 0;
            //virtual void handleInput() = 0;


        protected:
            Camera* camera;
    };

    //void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
        //// TODO: make projection-independant movement
        //float velocity = movementSpeed * deltaTime;
        //switch(direction) {
            //case CameraMovement::FORWARD:
                //position += front * velocity;
                //break;
            //case CameraMovement::BACKWARD:
                //position -= front * velocity;
                //break;
            //case CameraMovement::LEFT:
                //position -= right * velocity;
                //break;
            //case CameraMovement::RIGHT:
                //position += right * velocity;
                //break;
            //case CameraMovement::UP:
                //position.y += velocity;
                //break;
            //case CameraMovement::DOWN:
                //position.y -= velocity;
                //break;

            //default:
                //break;
        //}
    //}

}
