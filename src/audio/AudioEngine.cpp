#include "AudioEngine.hpp"

#include "Logger.hpp"

#include "sndfile.h"

namespace Villain {

    AudioEngine::AudioEngine() {
        // Initialize OpenAL
        device = alcOpenDevice(nullptr); // Opens the default audio device
        if (!device) {
            VILLAIN_ERROR("Failed to initialize OpenAL device");
        }

        context = alcCreateContext(device, nullptr);
        if (!context) {
            VILLAIN_ERROR("Failed to create OpenAL context");
            alcCloseDevice(device);
        }

        if (!alcMakeContextCurrent(context)) {
            VILLAIN_ERROR("Failed to make OpenAL context current");
            alcDestroyContext(context);
            alcCloseDevice(device);
        }
    }

    AudioEngine::~AudioEngine() {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

    void AudioEngine::update(const glm::vec3& listenerPos, const glm::vec3& listenerForward, const glm::vec3& listenerUp) {
        setListenerPosition(listenerPos);
        setListenerOrientation(listenerForward, listenerUp);
    }

    void AudioEngine::setListenerPosition(const glm::vec3& pos) {
        alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
    }

    void AudioEngine::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) {
        ALfloat listenerOrientation[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
        alListenerfv(AL_ORIENTATION, listenerOrientation);
    }
}
