#ifndef __PHYSICS_ENGINE__
#define __PHYSICS_ENGINE__

#include "PhysicsObject.hpp"
#include "rendering/DebugRenderer.hpp"
#include <vector>

namespace Villain {

    class Engine;

    class PhysicsEngine {
        public:
            PhysicsEngine(Engine* e): engine(e), debugMode(true) {
                debugRenderer = new DebugRenderer();
                debugRenderer->init();
            }

            ~PhysicsEngine() {
                delete debugRenderer;
            }

            void addObject(const PhysicsObject& object);
            void simulate(float deltaTime);

            // Temporarily for now both collision detection and response done in same step
            void handleCollisions();

            void setDebugMode(bool debug) { debugMode = debug; }
            void render();

            // TEMP
            inline const PhysicsObject& getObject(unsigned int index) const { return objects[index]; }
            inline unsigned int getNumObjects() const { return objects.size(); }
        private:
            Engine* engine = nullptr;
            std::vector<PhysicsObject> objects;
            // TODO:: add new 'Solver' class, to calculate collision responses
            DebugRenderer* debugRenderer = nullptr;
            bool debugMode = true;
    };
}

#endif // __PHYSICS_ENGINE__
