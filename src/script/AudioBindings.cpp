#include "AudioBindings.hpp"

#include "audio/AudioSource.hpp"
#include "LuaBindings.hpp"

namespace Villain {

    void AudioBindings::registerBindings(lua_State* L) {
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

    int AudioBindings::lua_AudioSource_isPlaying(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        lua_pushboolean(L, source->isPlaying());
        return 1;
    }

    int AudioBindings::lua_AudioSource_play(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        source->play();
        return 0;
    }

    int AudioBindings::lua_AudioSource_pause(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        source->pause();
        return 0;
    }

    int AudioBindings::lua_AudioSource_stop(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        source->stop();
        return 0;
    }

    int AudioBindings::lua_AudioSource_rewind(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        source->rewind();
        return 0;
    }

    int AudioBindings::lua_AudioSource_setVolume(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        float volume = static_cast<float>(lua_tonumber(L, 2));
        source->setVolume(volume);
        return 0;
    }

    int AudioBindings::lua_AudioSource_setPitch(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        float pitch = static_cast<float>(lua_tonumber(L, 2));
        source->setPitch(pitch);
        return 0;
    }

    int AudioBindings::lua_AudioSource_setLooping(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        bool loop = lua_toboolean(L, 2);
        source->setLooping(loop);
        return 0;
    }

    int AudioBindings::lua_AudioSource_setPositions(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        glm::vec3 position = LuaBindings::readVec3FromLua(L, 2);
        glm::vec3 listenerPos = LuaBindings::readVec3FromLua(L, 3);
        source->setPositionDirection(position, listenerPos);
        return 0;
    }

    int AudioBindings::lua_AudioSource_setVelocity(lua_State* L) {
        AudioSource* source = static_cast<AudioSource*>(lua_touserdata(L, 1));
        glm::vec3 velocity = LuaBindings::readVec3FromLua(L, 2);
        source->setVelocity(velocity);
        return 0;
    }
}
