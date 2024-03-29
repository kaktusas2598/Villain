#pragma once

#include "AudioBuffer.hpp"
#include "glm/glm.hpp"

namespace Villain {

    /// Represents an audio source (speaker)
    class AudioSource {
        public:
            AudioSource(AudioBuffer* buffer);
            ~AudioSource();

            /// Playback Controls
            bool isPlaying();
            void play();
            void pause();
            void stop();
            void rewind();

            /// Volume control (0.0 to +inf) (default 1.0)
            void setVolume(float volume);
            /// Pitch(speed) control (0.0 to +inf) (default 1.0)
            void setPitch(float pitch);
            /// Set if audio source should loop or not
            void setLooping(bool loop);
            /// Setup distance attenation - reference distance is distance where sound will be heard at original volume
            /// max distance at which sound will be heard
            void setDistanceAttenuation(float referenceDistance, float maxDistance);

            /// 3D sound position and orientation setters
            void setPosition(const glm::vec3& position);
            void setDirection(const glm::vec3& direction);
            void setPositionDirection(const glm::vec3& position, const glm::vec3& listenerPosition);
            void setVelocity(const glm::vec3& velocity);

        private:
            ALuint sourceId;

    };
}
