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

    void AudioEngine::setListenerPosition(ALfloat x, ALfloat y, ALfloat z) {
        alListener3f(AL_POSITION, x, y, z);
    }

    void AudioEngine::setListenerOrientation(ALfloat forwardX, ALfloat forwardY, ALfloat forwardZ, ALfloat upX, ALfloat upY, ALfloat upZ) {
        ALfloat listenerOrientation[] = { forwardX, forwardY, forwardZ, upX, upY, upZ };
        alListenerfv(AL_ORIENTATION, listenerOrientation);
    }
}
