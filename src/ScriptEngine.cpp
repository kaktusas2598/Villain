#include "ScriptEngine.hpp"

#include "Logger.hpp"

#include <cassert>
#include <iostream>

namespace Villain {

    ScriptEngine* ScriptEngine::s_pInstance = nullptr;

    /*int ScriptEngine::lua_createEntity(lua_State *L) {
        // Important to understand here that memory is being allocated by Lua and
        // we want to pass it to EntityManager so it can update and render all entities
        Entity* entity = (Entity*)lua_newuserdata(L, sizeof(Entity));
        // Placement new operator takes already allocated memory and calls constructor
        new (entity) Entity();

        EntityManager::Instance()->addEntity(entity);

        luaL_getmetatable(L, "EntityMetaTable");
        lua_setmetatable(L, -2);

        return 1;
    }

    // Will be called once lua state is closed
	int ScriptEngine::lua_destroyEntity(lua_State *L) {
		Entity* entity = (Entity*)lua_touserdata(L, -1);
		//std::cout << "Lua __gc for Entity called" << std::endl;
		// Lua takes care of gc and entity gets removed from entity manager using its id
		// to avoid double freeing of same memory, so we don't need to call destructor explicitly
		// Tried maping entities with ids using map in entity manager but could not get that to work
		EntityManager::Instance()->remove(entity->id->get());
		entity->clean();

		return 0;
	}*/


    ScriptEngine::ScriptEngine() {
        build();
    }

    void ScriptEngine::build() {
        state = luaL_newstate();

        /*lua_newtable(state);
        lua_setglobal(state, "entities");

        // Setup table for Entity class with __index and __gc metamethods
        lua_newtable(state);
        int entityTableIndex = lua_gettop(state);
        // Push entity table twice because lua_setglobal will pop it off the stack, but we will need table to assign methods
        lua_pushvalue(state, entityTableIndex);
        lua_setglobal(state, "Entity");

        // Set methods for Entity table
        lua_pushcfunction(state, lua_createEntity);
        lua_setfield(state, -2 , "create");
        lua_pushcfunction(state, lua_removeEntity);
        lua_setfield(state, -2 , "remove");
        lua_pushcfunction(state, lua_registerListener);
        lua_setfield(state, -2 , "registerListener");
        // ID Component
        lua_pushcfunction(state, lua_entityId);
        lua_setfield(state, -2 , "id");
        lua_pushcfunction(state, lua_getTag);
        lua_setfield(state, -2 , "getTag");
        lua_pushcfunction(state, lua_setTag);
        lua_setfield(state, -2 , "setTag");
        lua_pushcfunction(state, lua_getType);
        lua_setfield(state, -2 , "getType");
        lua_pushcfunction(state, lua_setType);
        lua_setfield(state, -2 , "setType");

        // Transform Component
        lua_pushcfunction(state, lua_teleportEntity);
        lua_setfield(state, -2 , "move");
        lua_pushcfunction(state, lua_scaleEntity);
        lua_setfield(state, -2 , "scale");

        lua_pushcfunction(state, lua_addCharacter);
        lua_setfield(state, -2 , "addCharacter");
        lua_pushcfunction(state, lua_entityHp);
        lua_setfield(state, -2 , "hp");
        lua_pushcfunction(state, lua_entityXp);
        lua_setfield(state, -2 , "xp");
        lua_pushcfunction(state, lua_entityLevel);
        lua_setfield(state, -2 , "level");
        lua_pushcfunction(state, lua_damage);
        lua_setfield(state, -2 , "damage");

        lua_pushcfunction(state, lua_addSprite);
        lua_setfield(state, -2 , "addSprite");
        lua_pushcfunction(state, lua_setAbsolutePosition);
        lua_setfield(state, -2 , "setAbsolutePosition");
        lua_pushcfunction(state, lua_addAnimation);
        lua_setfield(state, -2 , "addAnimation");
        lua_pushcfunction(state, lua_setAnimation);
        lua_setfield(state, -2 , "setAnimation");

        lua_pushcfunction(state, lua_addCollider);
        lua_setfield(state, -2 , "addCollider");

        lua_pushcfunction(state, lua_addInput);
        lua_setfield(state, -2 , "addInput");
        lua_pushcfunction(state, lua_addBehaviour);
        lua_setfield(state, -2 , "addBehaviour");
        lua_pushcfunction(state, lua_addMoveBehaviour);
        lua_setfield(state, -2 , "addMoveBehaviour");

        lua_pushcfunction(state, lua_addButton);
        lua_setfield(state, -2 , "addButton");
        lua_pushcfunction(state, lua_addBackground);
        lua_setfield(state, -2 , "addBackground");
        lua_pushcfunction(state, lua_addLabel);
        lua_setfield(state, -2 , "addLabel");
        lua_pushcfunction(state, lua_setLabel);
        lua_setfield(state, -2 , "setLabel");
        lua_pushcfunction(state, lua_setAlignment);
        lua_setfield(state, -2 , "setAlignment");

        lua_pushcfunction(state, lua_addPhysics);
        lua_setfield(state, -2 , "addPhysics");
        lua_pushcfunction(state, lua_addProjectile);
        lua_setfield(state, -2 , "addProjectile");
        lua_pushcfunction(state, lua_applyForceX);
        lua_setfield(state, -2 , "applyForceX");
        lua_pushcfunction(state, lua_applyForceY);
        lua_setfield(state, -2 , "applyForceY");

        // TODO: replace this with __newindex?
        lua_pushcfunction(state, lua_getX);
        lua_setfield(state, -2 , "getX");
        lua_pushcfunction(state, lua_getY);
        lua_setfield(state, -2 , "getY");

        luaL_newmetatable(state, "EntityMetaTable");

        // So Lua knows how to automatically garbage collect entities
        lua_pushstring(state, "__gc");
        lua_pushcfunction(state, lua_destroyEntity);
        lua_settable(state, -3);

        // Will let use Entity OOP way in lua
        lua_pushstring(state, "__index");
        lua_pushvalue(state, entityTableIndex);
        lua_settable(state, -3);*/
        // Also might want to implement __newindex to write properties oop way eg. entity.x = 10
        // Embedding Lua in C++ #13 tutorial

        // Register C++ -> Lua wrappers
        // Must be done before running lua script
        //lua_register(state, "playSound", lua_playSound);
        //lua_register(state, "playMusic", lua_playMusic);
        //lua_register(state, "addParticleEmitter", lua_addParticleEmitter);
        //lua_register(state, "getMapWidth", lua_getMapWidth);
        //lua_register(state, "getMapHeight", lua_getMapHeight);
        //lua_register(state, "getCameraX", lua_getCameraX);
        //lua_register(state, "getCameraY", lua_getCameraY);
        //lua_register(state, "changeState", lua_changeState);
        lua_register(state, "quit", lua_quit);
        lua_register(state, "addLog", lua_addLog);
    }

    void ScriptEngine::init(std::string fileName) {
        //id = tag;
        //fileName = filename;
        script = new LuaScript(fileName);
        state = script->getLuaState();

        build();

        script->open();
    }

    void ScriptEngine::loadScript(std::string fileName) {
        if (luaL_loadfile(state, fileName.c_str()) || lua_pcall(state, 0, 0, 0)) {
            std::string errorMessage = lua_tostring(state, -1);
            std::cout << errorMessage << std::endl;
            Logger::Instance()->error(errorMessage.c_str());
        }
    }

    void ScriptEngine::loadChunk(const char* chunk) {
        if (luaL_loadstring(state, chunk) || lua_pcall(state, 0, 0, 0)) {
            std::string errorMessage = lua_tostring(state, -1);
            std::cout << errorMessage << std::endl;
            Logger::Instance()->error(errorMessage.c_str());
        }
    }

    std::string ScriptEngine::runChunk(const char* chunk) {
        // Clean Lua stack before running another chunk
        int n = lua_gettop(state);
        lua_pop(state, n);

        if (luaL_dostring (state, chunk)) {
            std::string errorMessage = lua_tostring(state, -1);
            return errorMessage;
        }
        // Else if no error check for other output
        if (lua_isstring(state, -1)) {
            std::string output = lua_tostring(state, -1);
            return output;
        }
        // Return an empty string if not output, any print() calls will be redirected to loggers in C++
        return std::string();
    }


    void ScriptEngine::update(float deltaTime) {
        // luaL_dofile(state, fileName.c_str());
    }

    void ScriptEngine::dispatch(const std::string listener, int firstParam, int secondParam) {
        // Find appropriate lua function and call
        if (!listener.empty()){
            lua_getglobal(state, listener.c_str());
            lua_pushnumber(state, firstParam);
            // Need a better way to handle params, this would break if we actually want to pass -1
            if (secondParam != -1) {
                lua_pushnumber(state, secondParam);
            }

            if (luaOk(state, lua_pcall(state, 2, 0, 0))) {
                //std::cout << "Listener invoked successfully" << std::endl;
            }
        }
    }

    void ScriptEngine::issueNextTask(int id) {
        lua_getglobal(state, "issueNextTask");
        if (lua_isfunction(state, -1)) {
            lua_pushnumber(state, id);
            if(!luaOk(state, lua_pcall(state, 1, 0, 0))) {
                // behaviour function error or script error
            }
        }
    }
}
