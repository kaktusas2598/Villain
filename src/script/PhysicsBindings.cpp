#include "PhysicsBindings.hpp"

#include "LuaBindings.hpp"
#include "physics/RigidBody.hpp"

namespace Villain {

    void PhysicsBindings::registerBindings(lua_State* L) {
            luaL_Reg rigidBodyMethods[] = {
            { "addForce", lua_RigidBody_addForce },
            { "addForceAtPoint", lua_RigidBody_addForceAtPoint },
            { "getAwake", lua_RigidBody_getAwake },
            { "getMass", lua_RigidBody_getMass },
            { "getInverseMass", lua_RigidBody_getInverseMass },
            { "setPosition", lua_RigidBody_setPosition },
            { "setOrientation", lua_RigidBody_setOrientation },
            { "addAngularVelocity", lua_RigidBody_addAngularVelocity },
            { "addLinearVelocity", lua_RigidBody_addLinearVelocity },
            { NULL, NULL } // The array must be null-terminated
        };

        if (luaL_newmetatable(L, "RigidBody")) {
            luaL_setfuncs(L, rigidBodyMethods, 0);

            lua_pushvalue(L, -1);
            lua_setfield(L, -2, "__index");
        }
    }

    int PhysicsBindings::lua_RigidBody_addForce(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        glm::vec3 force = LuaBindings::readVec3FromLua(L, 2);
        rb->addForce(force);
        return 0;
    }

    int PhysicsBindings::lua_RigidBody_addForceAtPoint(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        glm::vec3 force = LuaBindings::readVec3FromLua(L, 2);
        glm::vec3 point = LuaBindings::readVec3FromLua(L, 3);
        rb->addForceAtPoint(force, point);
        return 0;
    }

    int PhysicsBindings::lua_RigidBody_getAwake(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        lua_pushboolean(L, rb->getAwake());
        return 1;
    }

    int PhysicsBindings::lua_RigidBody_getMass(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        lua_pushnumber(L, rb->getMass());
        return 1;
    }

    int PhysicsBindings::lua_RigidBody_getInverseMass(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        lua_pushnumber(L, rb->getInverseMass());
        return 1;
    }

    int PhysicsBindings::lua_RigidBody_setPosition(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        glm::vec3 pos = LuaBindings::readVec3FromLua(L, 2);
        rb->setPosition(pos);
        return 0;
    }

    int PhysicsBindings::lua_RigidBody_setOrientation(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        glm::vec3 eulerAngles = LuaBindings::readVec3FromLua(L, 2);
        // Convert euler angles to a quaternion
        glm::quat orientation = glm::quat(eulerAngles);
        rb->setOrientation(orientation);
        return 0;
    }

    int PhysicsBindings::lua_RigidBody_addAngularVelocity(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        glm::vec3 angularVel = LuaBindings::readVec3FromLua(L, 2);
        rb->addAngularVelocity(angularVel);
        return 0;
    }

    int PhysicsBindings::lua_RigidBody_addLinearVelocity(lua_State* L) {
        RigidBody* rb = static_cast<RigidBody*>(lua_touserdata(L, 1));
        glm::vec3 linearVel = LuaBindings::readVec3FromLua(L, 2);
        rb->addLinearVelocity(linearVel);
        return 0;
    }
}
