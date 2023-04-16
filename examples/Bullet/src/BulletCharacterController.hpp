#pragma once

#include "NodeComponent.hpp"
#include "btBulletDynamicsCommon.h"

// TODO:
class BulletCharacterController: /*public Villain::NodeComponent,*/ public btActionInterface {
    public:
        BulletCharacterController(btRigidBody *body, const btCapsuleShape *shape);
        virtual ~BulletCharacterController() = default;

        virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep);
        virtual void debugDraw(btIDebugDraw* debugDrawer);

        void setMovementDirection(const btVector3 &walkDirection);
        const btVector3 &getMovementDirection() const { return moveDirection; }
        void jump(const btVector3 &dir);
        const btRigidBody *getBody() const { return rigidBody; }

        btScalar shapeRadius; // The radius of the capsule
        btScalar shapeHalfHeight; // Half of the height of the capsule

        /**
         * Ground detection works with collision checking.
         * We take all the collisions of the rigid body and we convert them to
         * spherical coordinates (with theta = 0 as up vector, like ISO standard).
         *
         * We check for the radius and the theta angle of that point, or, to be more
         * specific, at its cosine. The character is considered on ground if these
         * values are within a certain threshold.
         * For the angle, by default, it must be between 0.75 pi and pi.
         *
         * This cosine is expected to be negative.
         */
        btScalar maxCosGround = -SIMDSQRT12;

        /**
         * The threshold on the radius, to tell if a character is on ground.
         * By default, 1cm (if using 1unit as 1m).
         * \sa mMaxCosGround
         */
        btScalar radiusThreshold = 1e-2;
    private:
        void setupBody();
        void resetStatus(); // Reset the movement status, but not the position.
        bool canJump() const { return onGround; }
        void updateVelocity(float dt);
        btRigidBody* rigidBody; // Body being controlled
        bool  shouldJump; // Whether to jump on next frame update
        btVector3 jumpDir; // Should be normalized
        bool onGround = false;
        btVector3 groundPoint; // Contact with ground point only valid when onGround = true
        btVector3 moveDirection;
        // The squared maximum speed that the character can walk. By default, 15km/h.
        btScalar maxLinearVelocity2 = pow(15 / 3.6, 2);
        btScalar walkAccel = 25.0; // Start walking acceleration

        // The initial speed for the jump.
        // The default(4.5) is about 1m starting still, with g=-9.81
        btScalar jumpSpeed = 4.5;
        btScalar speedDamping = 0.1; // Damping used to stop movements
        btVector3 gravity;
};
