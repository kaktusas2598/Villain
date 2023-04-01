#ifndef __IGAMESCREEN__
#define __IGAMESCREEN__

#include "Application.hpp"

namespace Villain {

    //class Engine;

    /**
     * @brief Enumeration of available states per GameState
     */
    enum class ScreenState {
        NONE,
        RUNNING, //< state is running
        EXIT_APPLICATION, //< exit application
        CHANGE_NEXT, ///< change to next screen
        CHANGE_PREVIOUS ///< change to previous screen
    };

    /*
     * Game Screen interface, nearly identical to GameState in Vigilant
     * NOTE: now would be good time to port StateLoader from Vigilant and add state XML and Lua abilities
     */
    class IGameScreen {
        public:
            friend class StateMachine;

            IGameScreen () {}
            //GameState(const std::string& id) {}
            virtual ~IGameScreen () {}

            // Return indices for next/previous screens
            virtual int getNextScreenID() const = 0;
            virtual int getPrevScreenID() const = 0;

            // Called on beggining and end of application
            virtual void build() = 0;
            virtual void destroy() = 0;

            // Called on entering/exiting focus
            virtual void onEntry() = 0;
            virtual void onExit() = 0;

            // Called in main app loop
            virtual void update(float deltaTime) = 0;
            virtual void draw() = 0;

            virtual void onImGuiDraw(float deltaTime) {}

            void setParentGame(Application* app) { mainApplication = app; }
            void setRunning() { currentScreenState = ScreenState::RUNNING; }
            void setScreenState(ScreenState state) { currentScreenState = state; }

            int getID() const { return screenID; }
            ScreenState getScreenState() const { return currentScreenState; }


        protected:
            int screenID = -1;

            Application* mainApplication = nullptr;
            ScreenState currentScreenState = ScreenState::NONE;
    };

    /*
     * Screen state parsed from XML file
     */
    class CustomXMLScreen : public IGameScreen {
        public:
            CustomXMLScreen();
            ~CustomXMLScreen();

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

}
#endif // __IGAMESCREEN__
