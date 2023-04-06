#ifndef __PHYSICS_ENGINE__
#define __PHYSICS_ENGINE__

#include "PhysicsObject.hpp"
#include <vector>

namespace Villain {

    class PhysicsEngine {
        public:
            PhysicsEngine() {}

            void addObject(const PhysicsObject& object);
            void simulate(float deltaTime);

            // TEMP
            inline const PhysicsObject& getObject(unsigned int index) const { return objects[index]; }
            inline unsigned int getNumObjects() const { return objects.size(); }
        private:
            std::vector<PhysicsObject> objects;
    };
}

#endif // __PHYSICS_ENGINE__
