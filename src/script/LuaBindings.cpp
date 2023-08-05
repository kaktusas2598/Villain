#include "LuaBindings.hpp"

#include "Engine.hpp"

#include "KeyCode.hpp"
#include "ResourceManager.hpp"
#include "audio/AudioSource.hpp"
#include "components/RigidBodyComponent.hpp"

namespace Villain {

    void LuaBindings::registerBindings(lua_State* L) {
        // Register C++ -> Lua wrappers
        // NOTE: Must be done before running lua script
        lua_register(L, "GetAudio", lua_getAudio);
        lua_register(L, "GetScreenWidth", lua_getScreenWidth);
        lua_register(L, "GetScreenHeight", lua_getScreenHeight);
        lua_register(L, "AddLog", lua_addLog);
        lua_register(L, "KeyCode", lua_keyCode);
        lua_register(L, "Quit", lua_quit);

        createAudioSourceMetatable(L);
        createSceneNodeMetatable(L);
    }

    void LuaBindings::createSceneNodeMetatable(lua_State* L) {
        // Set methods for the metatable
        luaL_Reg sceneNodeMethods[] = {
            { "getName", lua_SceneNode_getName },
            { "getComponent", lua_SceneNode_getComponent },
            { "getPosition", lua_SceneNode_getPosition },
            { "setPosition", lua_SceneNode_setPosition },
            { NULL, NULL } // The array must be null-terminated
        };

        if (luaL_newmetatable(L, "SceneNode")) {
            luaL_setfuncs(L, sceneNodeMethods, 0);

            // Set "__index" to point to the metatable itself so that we can access methods
            // from the metatable directly on userdata
            lua_pushvalue(L, -1);
            lua_setfield(L, -2, "__index");
        }
    }

    void LuaBindings::createAudioSourceMetatable(lua_State* L) {
        luaL_Reg audioSourceMethods[] = {
            { "isPlaying", lua_AudioSource_isPlaying },
            { "play", lua_AudioSource_play },
            { "pause", lua_AudioSource_pause },
            { "stop", lua_AudioSource_stop },
            { "rewind", lua_AudioSource_rewind },
            { "setVolume", lua_AudioSource_setVolume },
            { "setPitch", lua_AudioSource_setPitch },
            { "setLooping", lua_AudioSource_setLooping },
            { "setPositions", lua_AudioSource_setPositions },
            { "setVelocity", lua_AudioSource_setVelocity },
            { NULL, NULL } // The array must be null-terminated
        };

        if (luaL_newmetatable(L, "AudioSource")) {
            luaL_setfuncs(L, audioSourceMethods, 0);

            lua_pushvalue(L, -1);
            lua_setfield(L, -2, "__index");
        }
    }

    int LuaBindings::lua_AudioSource_isPlaying(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        lua_pushboolean(L, source->isPlaying());
        return 1;
    }

    int LuaBindings::lua_AudioSource_play(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        source->play();
        return 0;
    }

    int LuaBindings::lua_AudioSource_pause(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        source->pause();
        return 0;
    }

    int LuaBindings::lua_AudioSource_stop(lua_State* L) {
AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        source->stop();
        return 0;
    }

    int LuaBindings::lua_AudioSource_rewind(lua_State* L) {
AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        source->rewind();
        return 0;
    }

    int LuaBindings::lua_AudioSource_setVolume(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        float volume = static_cast<float>(lua_tonumber(L, 2));
        source->setVolume(volume);
        return 0;
    }

    int LuaBindings::lua_AudioSource_setPitch(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        float pitch = static_cast<float>(lua_tonumber(L, 2));
        source->setPitch(pitch);
        return 0;
    }

    int LuaBindings::lua_AudioSource_setLooping(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        bool loop = lua_toboolean(L, 2);
        source->setLooping(loop);
        return 0;
    }

    int LuaBindings::lua_AudioSource_setPositions(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        glm::vec3 position = readVec3FromLua(L, 2);
        glm::vec3 listenerPos = readVec3FromLua(L, 3);
        source->setPositionDirection(position, listenerPos);
        return 0;
    }

    int LuaBindings::lua_AudioSource_setVelocity(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        glm::vec3 velocity = readVec3FromLua(L, 2);
        source->setVelocity(velocity);
        return 0;
    }

    int LuaBindings::lua_SceneNode_getName(lua_State *L) {
        SceneNode* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
        lua_pushstring(L, node->getName().c_str());
        return 1;
    }

    int LuaBindings::lua_SceneNode_getComponent(lua_State *L) {
        SceneNode* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
        const char* componentName = luaL_checkstring(L, 2);

        if (strcmp(componentName, "Camera") == 0) {
            Camera* camera = node->getComponent<Camera>();
            if (camera) {
                // TODO: decide how we will represent this: userdata, lightuserdata?
                lua_pushboolean(L, true);
                return 1;
            }
        }

        lua_pushnil(L);
        return 1;
    }

    int LuaBindings::lua_SceneNode_getPosition(lua_State *L) {
        SceneNode* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
        glm::vec3 position = node->getTransform()->getPos();

        pushVec3ToLua(L, position);
        return 1;
    }

    int LuaBindings::lua_SceneNode_setPosition(lua_State *L) {
        SceneNode* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
        glm::vec3 position = readVec3FromLua(L, 2);

        // Rigid body will override node position so set it explicitely if node has it
        if (node->getComponent<RigidBodyComponent>()) {
            RigidBodyComponent* rg = node->getComponent<RigidBodyComponent>();
            rg->getBody()->setPosition(position);
        } else {
            node->getTransform()->setPos(position);
        }

        return 0;
    }

    int LuaBindings::lua_getAudio(lua_State *L) {
        const char* fileName = luaL_checkstring(L, 1);
        AudioBuffer* buffer = ResourceManager::Instance()->getAudio(fileName);
        if (!buffer)
            buffer = ResourceManager::Instance()->loadAudio(fileName, fileName);
        //lua_pushlightuserdata(L, new AudioSource(buffer));
        // Create a new userdata to wrap the AudioSource
        AudioSource* userdata = new AudioSource(buffer);

        // Push the userdata to the Lua stack
        void* ptr = lua_newuserdata(L, sizeof(AudioSource));
        memcpy(ptr, userdata, sizeof(AudioSource));
        luaL_setmetatable(L, "AudioSource");
        return 1;
    }

    int LuaBindings::lua_getScreenWidth(lua_State *L) {
        int screenWidth = Engine::getScreenWidth();
        lua_pushnumber(L, screenWidth);
        return 1;
    }

    int LuaBindings::lua_getScreenHeight(lua_State *L) {
        int screenHeight = Engine::getScreenHeight();
        lua_pushnumber(L, screenHeight);
        return 1;
    }

    int LuaBindings::lua_addLog(lua_State *L) {
        int nArgs = lua_gettop(L);
        // TODO: support building variadic args
        for (int i = 0; i < nArgs; i++) {
            if (lua_isstring(L, i)) {
                DebugConsole::Instance()->addLog(lua_tostring(L,i));
            }
        }
        return 0;
    }

    int LuaBindings::lua_keyCode(lua_State *L) {
        const char* keyName = luaL_checkstring(L, 1);

        if (strcmp(keyName, "r") == 0) {
            lua_pushinteger(L, static_cast<int>(KeyCode::r));
        } else if (strcmp(keyName, "space") == 0) {
            lua_pushinteger(L, static_cast<int>(KeyCode::SPACE));
        } else if (strcmp(keyName, "enter") == 0) {
            lua_pushinteger(L, static_cast<int>(KeyCode::RETURN));
        } else {
            lua_pushinteger(L, static_cast<int>(KeyCode::UKNOWN));
        }
        // TODO: Add all the rest :/

        return 1;
    }

    int LuaBindings::lua_quit(lua_State *L) {
        Engine::setRunning(false);
        return 0;
    }

    void LuaBindings::pushVec3ToLua(lua_State* L, const glm::vec3& vec) {
        lua_newtable(L);
        lua_pushnumber(L, vec.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, vec.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, vec.z);
        lua_setfield(L, -2, "z");
    }

    glm::vec3 LuaBindings::readVec3FromLua(lua_State* L, int index) {
        glm::vec3 vec;

        lua_pushstring(L, "x");
        lua_gettable(L, index);
        vec.x = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "y");
        lua_gettable(L, index);
        vec.y = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "z");
        lua_gettable(L, index);
        vec.z = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        return vec;
    }
}
