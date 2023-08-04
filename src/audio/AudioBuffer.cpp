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

    void AudioBuffer::pause() {
        alSourcePause(sourceId);
    }

    void AudioBuffer::stop() {
        alSourceStop(sourceId);
    }

    void AudioBuffer::rewind() {
        alSourceRewind(sourceId);
    }

    void AudioBuffer::setVolume(float volume) {
        alSourcef(sourceId, AL_GAIN, volume);
    }

    void AudioBuffer::setPitch(float pitch) {
        alSourcef(sourceId, AL_PITCH, pitch);
    }

    void AudioBuffer::setLooping(bool loop) {
        alSourcei(sourceId, AL_LOOPING, loop ? AL_TRUE: AL_FALSE);
    }

    void AudioBuffer::setPosition(const glm::vec3& position) {
        alSource3f(sourceId, AL_POSITION, position.x, position.y, position.z);
    }


    void AudioBuffer::setDirection(const glm::vec3& direction) {
        alSource3f(sourceId, AL_DIRECTION, direction.x, direction.y, direction.z);
    }

    void AudioBuffer::setPositionDirection(const glm::vec3& position, const glm::vec3& listenerPosition) {
        setPosition(position);
        setDirection(glm::normalize(listenerPosition - listenerPosition));
    }

    void AudioBuffer::setVelocity(const glm::vec3& velocity) {
        alSource3f(sourceId, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }
}
