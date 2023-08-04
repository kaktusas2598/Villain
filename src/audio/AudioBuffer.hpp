#pragma once

#include "AL/al.h"
#include "glm/glm.hpp"
#include <string>

namespace Villain {

    // TODO: Audio callbacks, Doppler effect, better error handling, decrease volume over the distance
    /// Represents both OpenAL buffer and a source
    class AudioBuffer {
        public:
            AudioBuffer(const std::string& fileName);
            ~AudioBuffer();

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

            /// 3D sound position and orientation setters
            void setPosition(const glm::vec3& position);
            void setDirection(const glm::vec3& direction);
            void setPositionDirection(const glm::vec3& position, const glm::vec3& listenerPosition);
            void setVelocity(const glm::vec3& velocity);

        private:
            ALuint bufferId;
            ALuint sourceId;
    };
}
