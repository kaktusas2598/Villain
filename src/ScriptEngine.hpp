#pragma once

#include "LuaScript.hpp"

#include "Engine.hpp"

#include <string>

namespace Villain {

    // TODO:
    // Must support multiple scripts, global script for common functionality, config scripts
    // Might work similar to TextureManager, maybe recall to ScriptManager?
    // addScript(ScriptType)
    enum ScriptType {
        SCRIPT_CONFIG = 0,
        SCRIPT_GLOBALS = 1,
        SCRIPT_ENTITIES = 2
    };

    // TODO: Rething is this class is even needed anymore?? Only runChunk() (not working) used in DebugConsole and init() called by StateParser
    class ScriptEngine {
        public:
            ScriptEngine();
            ~ScriptEngine() {}

            static ScriptEngine* Instance() {
                if(s_pInstance == 0) {
                    s_pInstance = new ScriptEngine();
                    return s_pInstance;
                }
                return s_pInstance;
            }

            // Scripts will probably not run every frame so this will be empty method
            // Maybe this could be used to limit insane amount of times onCollide() event is triggered?
            //
            void update(float deltaTime);

            // Setup metatables and bind c++ methods
            void build();

            // Setup metatables and bind c++ methods, load script
            // Tried loading init script for all states, functions can be shared, but variables
            // don't seem to be shared even in the same lua_state?
            void init(std::string fileName);

            // Loads and runs script
            void loadScript(std::string fileName);
            void loadChunk(const char* chunk);
            std::string runChunk(const char* chunk); // Identical as loadChunk(), but returns any output

            // Close all scripts
            void close() {
                script->close();
            }

            // Dispatch event from the engine to lua by calling listener in lua
            // First param will always be entity Id, second param is optional
            // Will need a better way to pass different params
            void dispatch(const std::string listener, int firstParam, int secondParam = -1);

            // Issues next task for dynamic behaviours/coroutines
            void issueNextTask(int id);

            // Lua API is written in C so only static C++ class methods can be wrapped for lua
            // Alternative way would be to use global functions
            //------------------------------------------------------------------------------------
            //static int lua_addParticleEmitter(lua_State *L) {
                //float originX = (float)lua_tonumber(L, 1);
                //float originY = (float)lua_tonumber(L, 2);
                //std::string type = (std::string)lua_tostring(L, 3);
                //ParticleSystem::Instance()->addEmitter(Vector2D{originX, originY}, type);
                //return 0;
            //}


            //static int lua_getMapWidth(lua_State *L) {
                //int mapWidth = TheEngine::Instance()->getMapWidth();
                //lua_pushnumber(L, mapWidth);
                //return 1;
            //}

            //static int lua_getMapHeight(lua_State *L) {
                //int mapHeight = TheEngine::Instance()->getMapHeight();
                //lua_pushnumber(L, mapHeight);
                //return 1;
            //}


            //static int lua_getCameraX(lua_State *L) {
                //int x = TheEngine::Instance()->camera.x;
                //lua_pushnumber(L, x);
                //return 1;
            //}

            //static int lua_getCameraY(lua_State *L) {
                //int y = TheEngine::Instance()->camera.y;
                //lua_pushnumber(L, y);
                //return 1;
            //}


            //static int lua_changeState(lua_State *L) {
                //std::string id = (std::string)lua_tostring(L, 1);
                //TheEngine::Instance()->getStateMachine()->getCurrentState()->setNextID(id);
                //TheEngine::Instance()->getStateMachine()->getCurrentState()->setScreenState(ScreenState::CHANGE_NEXT);
                //return 1;
            //}

            static int lua_quit(lua_State *L) {
                //TheEngine::Instance()->setRunning(false);
                return 0;
            }

            static int lua_addLog(lua_State *L) {
                int nArgs = lua_gettop(L);
                // TODO: support building variadic args
                for (int i = 0; i < nArgs; i++) {
                    if (lua_isstring(L, i)) {
                        DebugConsole::Instance()->addLog(lua_tostring(L,i));
                    }
                }
                return 0;
            }


            // Allocates new user data and sends reference to EntityManager
            /*static int lua_createEntity(lua_State *L);
            // Lua __gc and removes reference from EntityManager
            static int lua_destroyEntity(lua_State *L);

            // Manual remove from entity manager array
            static int lua_removeEntity(lua_State *L) {
                Entity* entity = (Entity*)lua_touserdata(L, -1);
                EntityManager::Instance()->remove(entity->id->get());
                return 0;
            }

            static int lua_registerListener(lua_State *L) {
                Entity* entity = (Entity*)lua_touserdata(L, 1);
                std::string type = (std::string)lua_tostring(L, 2);
                std::string listener = (std::string)lua_tostring(L, 3);
                entity->registerListener(type, listener);
                return 0;
            }*/

            // Temporary place, this method should go to dedicated scripting classes
            bool luaOk(lua_State* L, int call) {
                if (call != LUA_OK) {
                    std::string errorMessage = lua_tostring(L, -1);
                    std::cout << errorMessage << std::endl;
                    Logger::Instance()->error(errorMessage.c_str());
                    return false;
                }
                return true;
            }
            //------------------------------------------------------------------------------------

        private:
            static ScriptEngine* s_pInstance;

            lua_State* state;
            // Used for init script and other scripts will load onto same state
            LuaScript* script;
            std::string fileName;
            // This identifies which table to get from lua ascript, dont like this for some reason
            std::string id;
    };
}
