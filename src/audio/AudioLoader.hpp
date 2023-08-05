#pragma once

#include <string>

namespace Villain {

    /// Helper audio loader utility using libsndfile
    class AudioLoader {

        enum SampleFormatType {
            Int16,
            Float,
            IMA4,
            MSADPCM
        };

        public:
            /// Loads audio formats supported by libsndfile
            static unsigned loadAudio(const std::string& fileName);
    };
}
