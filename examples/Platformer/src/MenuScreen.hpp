#ifndef __MENU_SCREEN__
#define __MENU_SCREEN__

#include "IGameScreen.hpp"
#include "rendering/SpriteBatch.hpp"
#include "ResourceManager.hpp"

class MenuScreen : public Villain::IGameScreen {
    public:
        MenuScreen();
        ~MenuScreen();

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
        virtual void draw() override;

    private:
};

#endif // __MENU_SCREEN__
