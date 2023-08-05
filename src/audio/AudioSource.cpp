#include "AudioSource.hpp"

#include "AudioLoader.hpp"
#include "Logger.hpp"

namespace Villain {

    AudioSource::AudioSource(AudioBuffer* buffer) {
        alGenSources(1, &sourceId);
        alSourcei(sourceId, AL_BUFFER, buffer->getID());
    }

    AudioSource::~AudioSource() {
        stop();
        alDeleteSources(1, &sourceId);
    }

    bool AudioSource::isPlaying() {
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    void AudioSource::play() {
        alSourcePlay(sourceId);
    }

    void AudioSource::pause() {
        alSourcePause(sourceId);
    }

    void AudioSource::stop() {
        alSourceStop(sourceId);
    }

    void AudioSource::rewind() {
        alSourceRewind(sourceId);
    }

    void AudioSource::setVolume(float volume) {
        alSourcef(sourceId, AL_GAIN, volume);
    }

    void AudioSource::setPitch(float pitch) {
        alSourcef(sourceId, AL_PITCH, pitch);
    }

    void AudioSource::setLooping(bool loop) {
        alSourcei(sourceId, AL_LOOPING, loop ? AL_TRUE: AL_FALSE);
    }

    void AudioSource::setDistanceAttenuation(float referenceDistance, float maxDistance) {
        alSourcef(sourceId, AL_REFERENCE_DISTANCE, referenceDistance);
        alSourcef(sourceId, AL_MAX_DISTANCE, maxDistance);
    }

    void AudioSource::setPosition(const glm::vec3& position) {
        alSource3f(sourceId, AL_POSITION, position.x, position.y, position.z);
    }

    void AudioSource::setDirection(const glm::vec3& direction) {
        alSource3f(sourceId, AL_DIRECTION, direction.x, direction.y, direction.z);
    }

    void AudioSource::setPositionDirection(const glm::vec3& position, const glm::vec3& listenerPosition) {
        setPosition(position);
        setDirection(glm::normalize(listenerPosition - position));
    }

    void AudioSource::setVelocity(const glm::vec3& velocity) {
        alSource3f(sourceId, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }
}
