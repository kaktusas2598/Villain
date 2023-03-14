#ifndef __SoundManager__
#define __SoundManager__

#include <SDL2/SDL_mixer.h>
#include <string>
#include <map>

namespace Villain {

    enum SoundType {
        SOUND_MUSIC = 0,
        SOUND_SFX = 1
    };

    class SoundManager {
        public:
            static SoundManager* Instance() {
                if (sInstance == 0) {
                    sInstance = new SoundManager();
                    return sInstance;
                }
                return sInstance;
            }

            bool load(std::string fileName, std::string id, SoundType type, int volumeAdjust = 0);
            void cleanSoundMaps();

            void playSound(std::string id, int loop = 0);
            void playMusic(std::string id, int loop = 0);

            // in SDL_Mixer music only plays on 1 global channel so methods below work for all music
            void pauseMusic();
            void stopMusic();
            void resumeMusic();
        private:
            static SoundManager* sInstance;

            std::map<std::string, Mix_Chunk*> sFXMap;
            std::map<std::string, Mix_Music*> musicMap;

            SoundManager();
            ~SoundManager();

            SoundManager(const SoundManager&);
            SoundManager &operator= (const SoundManager&);
    };
}

#endif // __SoundManager__
