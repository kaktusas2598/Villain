#ifndef __GAME_PLAY_SCREEN__
#define __GAME_PLAY_SCREEN__

#include "IGameScreen.hpp"

class GamePlayScreen : public Villain::IGameScreen {
    public:
        GamePlayScreen();
        ~GamePlayScreen();

        virtual int getNextScreenID() const override;
        virtual int getPrevScreenID() const override;

        // Called on beggining and end of application
        virtual void build() override;
        virtual void destroy() override;

        // Called on entering/exiting focus
        virtual void onEntry() override;
        virtual void onExit() override;

        // Called in main app loop
        virtual void update(float deltaTime) override;
        virtual void draw(float deltaTime) override;

    private:
};

#endif // __GAME_PLAY_SCREEN__
