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

    void AudioEngine::playWAV(ALuint audioBuffer) {
        // Create an OpenAL source and attach the buffer to it
        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, audioBuffer);

        // Play the sound
        alSourcePlay(source);

        // Wait until the sound finishes playing
        ALint state;
        do {
            alGetSourcei(source, AL_SOURCE_STATE, &state);
        } while (state == AL_PLAYING);

        // Clean up OpenAL resources
        alDeleteSources(1, &source);

        //TODO: when do we exactly clean the buffer?
        //alDeleteBuffers(1, &audioBuffer);
    }
}
