#ifndef __SCENE__
#define __SCENE__

#include "NodeComponent.hpp"
#include <vector>

namespace Villain {

    // Ideas borrowed from SceneGraph, each Node/Entity will have a bitmask to represent which components it has,
    // which will also make it easier to use it in imgui
    typedef unsigned long long EntityID;
    const int MAX_COMPONENTS = 32;
    typedef std::bitset<MAX_COMPONENTS> ComponentMask;

    class Scene {

        struct Entity {
            EntityID ID;
            ComponentMask Mask;
            //EntityID ParentId = ;
        };

        public:
            EntityID newEntity() {
                entities.push_back({entities.size(), ComponentMask()});
                return entities.back().ID;
            }

            template <typename T> void assign(EntityID id) {
                int componentId = GetId<T>();
                entities[id].Mask.set(componentId);
            }

        private:
        // Rename to Scene Node to entity
        std::vector<Entity> entities;
    };
}
#endif // __SCENE__
