#pragma once

#include <glm/glm.hpp>

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

namespace Villain {

    class PhysicsBindings {
        public:
            /// Must be called before executing script using these bindings
            static void registerBindings(lua_State* L);

        private:
            /// RigidBody metatable bindings
            static int lua_RigidBody_addForce(lua_State* L);
            static int lua_RigidBody_addForceAtPoint(lua_State* L);
            static int lua_RigidBody_getAwake(lua_State* L);
            static int lua_RigidBody_getMass(lua_State* L);
            static int lua_RigidBody_getInverseMass(lua_State* L);
            static int lua_RigidBody_setPosition(lua_State* L);
            static int lua_RigidBody_setOrientation(lua_State* L);
            static int lua_RigidBody_addAngularVelocity(lua_State* L);
            static int lua_RigidBody_addLinearVelocity(lua_State* L);
    };
}
