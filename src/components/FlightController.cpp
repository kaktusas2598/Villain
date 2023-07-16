#include "FlightController.hpp"
#include "Engine.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Villain {

    void FlightController::addToEngine(Engine* engine) {
        RigidBodyComponent::addToEngine(engine);

        // Setup aerodynamic control surfaces
        float wingMinTensor[9] = {
            -0.2, 0, 0,
            -0.2, -0.5, 0,
            0, 0, -0.1
        };
        float wingTensor[9] = {
            -0.1, 0, 0,
            1, -0.5, 0,
            0, 0, -0.1
        };
        float wingMaxTensor[9] = {
            -0.2, 0, 0,
            1.4, -0.5, 0,
            0, 0, -0.1
        };

        leftWing = new AeroControl(
                glm::make_mat3(wingTensor), glm::make_mat3(wingMinTensor), glm::make_mat3(wingMaxTensor),
                {-1.0, 0.0, -2.0}, windSpeed);

        rightWing = new AeroControl(
                glm::make_mat3(wingTensor), glm::make_mat3(wingMinTensor), glm::make_mat3(wingMaxTensor),
                {-1.0, 0.0, 2.0}, windSpeed);

        float rudderMinTensor[9] = {
            -0.1, 0, -0.4,
            0, -0.1, 0,
            0, 0, -0.5
        };
        float rudderTensor[9] = {
            -0.1, 0, 0,
            0, -0.1, 0,
            0, 0, -0.5
        };
        float rudderMaxTensor[9] = {
            -0.1, 0, 0.4,
            0.0, -0.1, 0,
            0, 0, -0.5
        };

        rudder = new AeroControl(
                glm::make_mat3(rudderTensor), glm::make_mat3(rudderMinTensor), glm::make_mat3(rudderMaxTensor),
                {2.0, 0.5, 0.0}, windSpeed);

        float tailTensor[9] = {
            -0.1, 0, 0,
            1, -0.5, 0,
            0, 0, -0.1
        };
        tail = new Aero(glm::make_mat3(tailTensor), {2.0, 0, 0}, windSpeed);

        resetPlane();

        body->setMass(2.5f);
        body->setDamping(0.8f, 0.8f);
        // TODO:
        float cuboidDepth = 10.0f;
        float cuboidWidth = 10.0f;
        float cuboidHeight = 5.0f;
        float cuboidInertiaX = (1.0f / 12.0f) * body->getMass() * (cuboidHeight * cuboidHeight + cuboidDepth * cuboidDepth);
        float cuboidInertiaY = (1.0f / 12.0f) * body->getMass() * (cuboidWidth * cuboidWidth + cuboidDepth * cuboidDepth);
        float cuboidInertiaZ = (1.0f / 12.0f) * body->getMass() * (cuboidWidth * cuboidWidth + cuboidHeight * cuboidHeight);
        float cuboidInertiaTensor[9] = {
            cuboidInertiaX, 0, 0,
            0, cuboidInertiaY, 0,
            0, 0, cuboidInertiaZ
        };
        body->setInertiaTensor(glm::make_mat3(cuboidInertiaTensor));
        body->calculateDerivedData();

        // FIXME: adding these and increasing throttle - plane accelerates wildly out of control
        engine->getRigidBodyWorld()->getForceRegistry().add(body, leftWing);
        engine->getRigidBodyWorld()->getForceRegistry().add(body, rightWing);
        engine->getRigidBodyWorld()->getForceRegistry().add(body, rudder);
        engine->getRigidBodyWorld()->getForceRegistry().add(body, tail);

        //engine->getRigidBodyWorld()->getForceRegistry().add(body, new Gravity(glm::vec3(0.0, -9.8, 0.0)));
    }

    void FlightController::update(float deltaTime) {
        RigidBodyComponent::update(deltaTime);

        // Hardcode propeller force
        if (throttle > 0) {
            body->calculateDerivedData();
            //glm::vec3 propellerForce = body->getTransform() * glm::vec4(-10.0f * throttle, 0.0f, 0.0f, 1.0f);
            glm::vec3 propellerForce = glm::normalize(body->getTransform() * glm::vec4(10.0f, 0.0f, 0.0f, 1.0f)) * throttle;
            //body->addForce(propellerForce);
            body->addForce({throttle, 0, 0});
            VILLAIN_TRACE("Throttle: {}", throttle);
            VILLAIN_TRACE("Propeller force: {}", glm::to_string(propellerForce));
        }
    }

    void FlightController::handleInput(float deltaTime) {
        //TODO:
        if (Input::Get()->isKeyDown(SDLK_LSHIFT)) {
            throttle += 0.01f * deltaTime;
        }
        if (Input::Get()->isKeyDown(SDLK_LCTRL)) {
            throttle -= 0.01f * deltaTime;
        }

        if (Input::Get()->isKeyDown(SDLK_q)) {
            rudderControl += 0.1f;
        }

        if (Input::Get()->isKeyDown(SDLK_e)) {
            rudderControl -= 0.1f;
        }

        if (Input::Get()->isKeyDown(SDLK_w)) {
            leftWingControl -= 0.1f;
            rightWingControl -= 0.1f;
        }

        if (Input::Get()->isKeyDown(SDLK_s)) {
            leftWingControl += 0.1f;
            rightWingControl += 0.1f;
        }

        if (Input::Get()->isKeyDown(SDLK_d)) {
            leftWingControl -= 0.1f;
            rightWingControl += 0.1f;
        }

        if (Input::Get()->isKeyDown(SDLK_a)) {
            leftWingControl += 0.1f;
            rightWingControl -= 0.1f;
        }

        // Reset plane controls
        if (Input::Get()->isKeyDown(SDLK_x)) {
            leftWingControl = 0.0f;
            rightWingControl = 0.0f;
            rudderControl = 0.0f;
        }

        // Reset plane position and orientation
        if (Input::Get()->isKeyDown(SDLK_r)) {
            resetPlane();
        }

        // Keep controls in range
        if (leftWingControl < -1.0) leftWingControl = -1.0;
        else if (leftWingControl > 1.0) leftWingControl = 1.0;
        if (rightWingControl < -1.0) rightWingControl = -1.0;
        else if (rightWingControl > 1.0) rightWingControl = 1.0;
        if (rudderControl < -1.0) rudderControl = -1.0;
        else if (rudderControl > 1.0) rudderControl = 1.0;


        if (leftWing) leftWing->setControl(leftWingControl);
        if (rightWing) rightWing->setControl(rightWingControl);
        if (rudder) rudder->setControl(rudderControl);
    }

    void FlightController::resetPlane() {
        body->setPosition({0, 0, 0});
        body->setOrientation({1, 0, 0, 0});
        body->setLinearVelocity({0, 0, 0});
        body->setAngularVelocity({0, 0, 0});
    }
}
