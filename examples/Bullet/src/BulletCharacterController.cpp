#include "BulletCharacterController.hpp"

#include "InputManager.hpp"
#include <algorithm>
using namespace Villain;


class FindGroundAndSteps : public btCollisionWorld::ContactResultCallback {
public:
	FindGroundAndSteps(const BulletCharacterController *control, const btCollisionWorld *w);

	btScalar addSingleResult(btManifoldPoint &cp,
		const btCollisionObjectWrapper *colObj0, int partId0, int index0,
		const btCollisionObjectWrapper *colObj1, int partId1, int index1);
	btVector3 getInvNormal();

	bool haveGround = false;
	btVector3 groundPoint;
	bool haveStep = false;
	btVector3 stepPoint;
	btVector3 stepNormal;

private:
	void checkGround(const btManifoldPoint &cp);

	const BulletCharacterController *controller;
	const btCollisionWorld *world;

	btScalar stepDist2;
};

BulletCharacterController::BulletCharacterController(btRigidBody *body, const btCapsuleShape *shape) {

	rigidBody = body;

	shapeRadius = shape->getRadius();
	shapeHalfHeight = shape->getHalfHeight();

    setupBody();
    resetStatus();
}

void BulletCharacterController::setupBody() {
	assert(rigidBody);
    // Disable unwanted rotations
    rigidBody->setSleepingThresholds(0.0, 0.0);
	rigidBody->setAngularFactor(0.0);

    gravity = rigidBody->getGravity();
    printf("Initial player gravity: %fX, %fY, %fZ\n", gravity.getX(), gravity.getY(), gravity.getZ());

    // MINE TEMP
    //rigidBody->setGravity({0.0, -9.8, 0.0});
    //rigidBody->setRollingFriction(1.0f);
    rigidBody->setFriction(1.5f);
}

void BulletCharacterController::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep) {
    FindGroundAndSteps groundSteps(this, collisionWorld);
    collisionWorld->contactTest(rigidBody, groundSteps);
	onGround = groundSteps.haveGround;
    groundPoint = groundSteps.groundPoint;

    updateVelocity(deltaTimeStep);
	//if (mStepping || groundSteps.mHaveStep) {
		//if (!mStepping) {
			//mSteppingTo = groundSteps.mStepPoint;
			//mSteppingInvNormal = groundSteps.getInvNormal();
		//}
		//stepUp(deltaTimeStep);
	//}

    if (onGround /*|| mStepping*/) {
        // Avoid going down on ramps, if already on ground, and clearGravity() is not enough
        rigidBody->setGravity({ 0, 0, 0 });
    } else {
        rigidBody->setGravity(gravity);
    }
}

void BulletCharacterController::updateVelocity(float dt) {
	btTransform transform;
	rigidBody->getMotionState()->getWorldTransform(transform);
	btMatrix3x3 &basis = transform.getBasis();
	/* Orthonormal basis - can just transpose to invert.
	Also Bullet does this in the btTransform::inverse */
	btMatrix3x3 inv = basis.transpose();

	btVector3 linearVelocity = inv * rigidBody->getLinearVelocity();

	if (moveDirection.fuzzyZero() && onGround) {
		linearVelocity *= speedDamping;
	} else if (onGround || linearVelocity[1] > 0) {
		btVector3 dv = moveDirection * (walkAccel * dt);
		linearVelocity += dv;

		btScalar speed2 = pow(linearVelocity.x(), 2)
			+ pow(linearVelocity.z(), 2);
		if (speed2 > maxLinearVelocity2) {
			btScalar correction = sqrt(maxLinearVelocity2 / speed2);
			linearVelocity[0] *= correction;
			linearVelocity[2] *= correction;
		}
	}

	if (shouldJump) {
		linearVelocity += jumpSpeed * jumpDir;
		shouldJump = false;
		//cancelStep();
	}

	rigidBody->setLinearVelocity(basis * linearVelocity);
}

void BulletCharacterController::debugDraw(btIDebugDraw* debugDrawer) {
    debugDrawer->drawCapsule(
            shapeRadius,
            shapeHalfHeight * 2.0f,
            1.0f,
            rigidBody->getWorldTransform(),
            btVector3(1.0f, 1.0f, 1.0f)
            );
    //if (mStepping) {
		//debugDrawer->drawContactPoint(mSteppingTo, { 0, 0, 1 }, 0, 1000,
			//{ 0, 0.3, 1 });
	//}
}

void BulletCharacterController::setMovementDirection(const btVector3 &walkDirection) {
	moveDirection = walkDirection;
    moveDirection.setY(0);
	if (!moveDirection.fuzzyZero()) {
		moveDirection.normalize();
	}
}

void BulletCharacterController::resetStatus() {
	moveDirection.setValue(0, 0, 0);
	shouldJump = false;
	onGround = false;
	//cancelStep();
}

void BulletCharacterController::jump(const btVector3 &dir) {
	if (!canJump()) {
		return;
	}

	shouldJump = true;

	jumpDir = dir;
	if (dir.fuzzyZero()) {
		jumpDir.setValue(0, 1, 0);
	}
	jumpDir.normalize();
}

FindGroundAndSteps::FindGroundAndSteps(const BulletCharacterController *control, const btCollisionWorld *w) {
    controller = control;
    world = w;
}

btScalar FindGroundAndSteps::addSingleResult(btManifoldPoint &cp,
    const btCollisionObjectWrapper *colObj0, int partId0, int index0,
    const btCollisionObjectWrapper *colObj1, int partId1, int index1) {

    if (colObj0->m_collisionObject == controller->getBody()) {
		/* The first object should always be the rigid body of the
		controller, but check anyway.
		In case the body is the second object, we cannot use the collision
		information, because Bullet provides only the normal of the second
		point. */
		checkGround(cp);
		//ResolveStepUp resolve(mController, mWorld, cp);
		//if (resolve.mIsStep) {
			//if (!mHaveStep || resolve.mDist2 < mStepDist2) {
				//mStepPoint = resolve.mRealPosWorld;
				//mStepNormal = cp.m_normalWorldOnB;
				//mStepDist2 = resolve.mDist2;
			//}
			//mHaveStep = true;
		//}
	}

	// By looking at btCollisionWorld.cpp, it seems Bullet ignores this value
	return 0;
}

btVector3 FindGroundAndSteps::getInvNormal() {
    printf("In getInvNormal()\n");
    if (!haveStep) {
		return btVector3(0, 0, 0);
	}
	btMatrix3x3 frame;
	// Build it as step to world
	frame[2].setValue(0, 1, 0);
	frame[1] = stepNormal;
	frame[0] = frame[1].cross(frame[2]);
	frame[0].normalize();
	// Convert it to world to step
	frame = frame.transpose();
	return frame[1];
}

void FindGroundAndSteps::checkGround(const btManifoldPoint &cp) {
    if (haveGround) {
		return;
	}

	btTransform inverse = controller->getBody()->getWorldTransform().inverse();
	btVector3 localPoint = inverse(cp.m_positionWorldOnB);
	localPoint[1] += controller->shapeHalfHeight;

	float r = localPoint.length();
	float cosTheta = localPoint[1] / r;

	if (fabs(r - controller->shapeRadius) <= controller->radiusThreshold
			&& cosTheta < controller->maxCosGround) {
		haveGround = true;
		groundPoint = cp.m_positionWorldOnB;
	}
}
