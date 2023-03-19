#ifndef __SCREEN_LIST__
#define __SCREEN_LIST__

#define STATE_NONE -1

#include <vector>

namespace Villain {

    class Engine;
    class IGameScreen;

    // NOTE: Similar to Vigilant's StateMachine which currently only supports states loaded from xml,
    // maybe would be nice to part some of that behaviour here and support both custom schemes in C++
    // and also Data Oriented screens/states (XML and/or Lua)

    /*! \brief StateMachine
     *         Finite State Machine Manager class.
     *
     * Finite State Machine. Manages std::vector of GameState instances
     * Limitations: changes between states sequentually
     *  @sa IGameScreen, Engine
     */

    class StateMachine {
        public:
            StateMachine (Engine* mainApplication);
            ~StateMachine ();

            IGameScreen* moveNext();
            IGameScreen* movePrev();

            void setScreen(int nextScreenID);
            //void setState(std::string nextStateID);
            void addScreen(IGameScreen* newScreen);

            void destroy();

            IGameScreen* getCurrentScreen();

        protected:
            Engine* mainApplication = nullptr;
            //std::map<std::string, GameState*> stateMap; ///< State collection mapped against state name
            std::vector<IGameScreen*> screens;
            int currentScreenID = STATE_NONE; ///< Indicates current state in states collection, -1 indicates none


    };
}

#endif // __SCREEN_LIST__
