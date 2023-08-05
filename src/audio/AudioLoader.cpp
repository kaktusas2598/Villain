#include "AudioLoader.hpp"

#include "Logger.hpp"

#include <AL/al.h>
#include <AL/alext.h>

#include <sndfile.h>

namespace Villain {

    unsigned AudioLoader::loadWAV(const std::string& fileName) {
        ALuint buffer;
        ALint byteblockalign = 0;
        ALint splblockalign = 0;
        SF_INFO fileInfo;
        enum SampleFormatType sampleFormat = Int16;
        // Load audio data using libsndfile
        SNDFILE* sndFile = sf_open(fileName.c_str(), SFM_READ, &fileInfo);

        // Error handling
        if (!sndFile) {
            VILLAIN_CRIT("Could not open audio in {}: {}", fileName, sf_strerror(sndFile));
        }

        // Check if audio is usable
        if(fileInfo.frames < 1) {
            VILLAIN_CRIT("Bad sample count in {} ", fileName);
            sf_close(sndFile);
            return 0;
        }

        switch (fileInfo.format) {
            case SF_FORMAT_PCM_24:
            case SF_FORMAT_PCM_32:
            case SF_FORMAT_FLOAT:
            case SF_FORMAT_DOUBLE:
            case SF_FORMAT_VORBIS:
            case SF_FORMAT_OPUS:
            case SF_FORMAT_ALAC_20:
            case SF_FORMAT_ALAC_24:
            case SF_FORMAT_ALAC_32:
            case 0x0080/*SF_FORMAT_MPEG_LAYER_I*/:
            case 0x0081/*SF_FORMAT_MPEG_LAYER_II*/:
            case 0x0082/*SF_FORMAT_MPEG_LAYER_III*/:
                if(alIsExtensionPresent("AL_EXT_FLOAT32"))
                    sampleFormat = SampleFormatType::Float;
                break;
            case SF_FORMAT_IMA_ADPCM:
                /* ADPCM formats require setting a block alignment as specified in the
                 * file, which needs to be read from the wave 'fmt ' chunk manually
                 * since libsndfile doesn't provide it in a format-agnostic way.
                 */
                if(fileInfo.channels <= 2 && (fileInfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV
                        && alIsExtensionPresent("AL_EXT_IMA4")
                        && alIsExtensionPresent("AL_SOFT_block_alignment"))
                    sampleFormat = SampleFormatType::IMA4;
                break;
            case SF_FORMAT_MS_ADPCM:
                if(fileInfo.channels <= 2 && (fileInfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV
                        && alIsExtensionPresent("AL_SOFT_MSADPCM")
                        && alIsExtensionPresent("AL_SOFT_block_alignment"))
                    sampleFormat = SampleFormatType::MSADPCM;
                break;
        }

        // Read audio data from libsndfile and populate the OpenAL buffer
        const sf_count_t numFrames = fileInfo.frames;
        const sf_count_t numChannels = fileInfo.channels;
        const sf_count_t numSamples = numFrames * numChannels;
        float* samples = new float[numSamples];
        ALenum format, err;

        if (numChannels == 1) {
            if(sampleFormat == Int16)
                format = AL_FORMAT_MONO16;
            else if(sampleFormat == Float)
                format = AL_FORMAT_MONO_FLOAT32;
            else if(sampleFormat == IMA4)
                format = AL_FORMAT_MONO_IMA4;
            else if(sampleFormat == MSADPCM)
                format = AL_FORMAT_MONO_MSADPCM_SOFT;
            if (sampleFormat == SampleFormatType::Float)
                format = AL_FORMAT_MONO_FLOAT32;
        } else if (numChannels == 2) {
            if(sampleFormat == Int16)
                format = AL_FORMAT_STEREO16;
            else if(sampleFormat == Float)
                format = AL_FORMAT_STEREO_FLOAT32;
            else if(sampleFormat == IMA4)
                format = AL_FORMAT_STEREO_IMA4;
            else if(sampleFormat == MSADPCM)
                format = AL_FORMAT_STEREO_MSADPCM_SOFT;
        } else {
            VILLAIN_CRIT("{} num of channels not implemented", numChannels);
            sf_close(sndFile);
            return 0;
        }

        // TODO: will have to read based on sample format so maybe sf_readf_short or sth
        sf_readf_float(sndFile, samples, numFrames);

        // Convert floating-point samples to 16-bit signed integer samples
        ALshort* bufferData = new ALshort[numSamples];
        for (int i = 0; i < numSamples; i++) {
            // Scale the floating-point sample to the range of a 16-bit signed integer (-32768 to 32767)
            bufferData[i] = static_cast<ALshort>(samples[i] * 32767.0f);
        }

        // Create OpenAL buffer to hold audio data
        alGenBuffers(1, &buffer);

        // Upload buffer and cleanup
        alBufferData(buffer, format, bufferData, numSamples * sizeof(ALshort), fileInfo.samplerate);
        delete[] samples;
        sf_close(sndFile);

        err = alGetError();
        if(err != AL_NO_ERROR) {
            fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
            if(buffer && alIsBuffer(buffer))
                alDeleteBuffers(1, &buffer);
            return 0;
        }

        return buffer;
    }
}
