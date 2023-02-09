#include "SoundManager.hpp"
#include "ErrorHandler.hpp"

namespace Villain {

    SoundManager* SoundManager::sInstance = 0;

    SoundManager::SoundManager() {
        // (int frequency, Uint16 format, int channels, int chunksize)
        Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
    }

    SoundManager::~SoundManager() {
        Mix_CloseAudio();
    }

    bool SoundManager::load(std::string fileName, std::string id, SoundType type) {
        if (type == SOUND_MUSIC) {
            if (musicMap.find(id) != musicMap.end()) {
                return true;
            }

            Mix_Music* music = Mix_LoadMUS(fileName.c_str());
            if (music == 0) {
                exitWithError("Could not load music file");
                return false;
            }
            musicMap[id] = music;
            return true;
        } else if (type == SOUND_SFX) {
            if (sFXMap.find(id) != sFXMap.end()) {
                return true;
            }

            Mix_Chunk* fx = Mix_LoadWAV(fileName.c_str());
            if (fx == 0) {
                exitWithError("Could not load wav file");
                return false;
            }
            sFXMap[id] = fx;
            return true;
        }
        return false;
    }

    void SoundManager::playSound(std::string id, int loop) {
        // -1 means any channel
        Mix_PlayChannel(-1, sFXMap[id], loop);
    }

    void SoundManager::playMusic(std::string id, int loop) {
        Mix_PlayMusic(musicMap[id], loop);
    }

    void SoundManager::cleanSoundMaps() {
        for (const auto& soundPair: sFXMap) {
            Mix_FreeChunk(sFXMap[soundPair.first]);
        }
        sFXMap.clear();
        for (const auto& soundPair: musicMap) {
            Mix_FreeMusic(musicMap[soundPair.first]);
        }
        musicMap.clear();
    }
}