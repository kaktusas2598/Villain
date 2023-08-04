#include "AudioBuffer.hpp"

#include "AudioLoader.hpp"

namespace Villain {

    AudioBuffer::AudioBuffer(const std::string& fileName) {
        bufferId = AudioLoader::loadWAV(fileName);

        alGenSources(1, &sourceId);
        alSourcei(sourceId, AL_BUFFER, bufferId);
    }

    AudioBuffer::~AudioBuffer() {
        // Clean up OpenAL resources
        alDeleteSources(1, &sourceId);
        alDeleteBuffers(1, &bufferId);
    }

    void AudioBuffer::play() {
        // Play the sound
        alSourcePlay(sourceId);

        // Wait until the sound finishes playing
        // TODO: will have to use this later on and implement callback support 100%
        //ALint state;
        //do {
            //alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        //} while (state == AL_PLAYING);
    }
}
