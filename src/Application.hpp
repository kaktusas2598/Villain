#ifndef __APPLICATION__
#define __APPLICATION__

#include "Engine.hpp"
#include "SceneNode.hpp"
#include "StateMachine.hpp"

#include <memory>

namespace Villain {

    class IGameScreen;

    // Every application must extend from this class
    class Application {
        public:
            Application();
            virtual ~Application() {}

            virtual void init() {}
            virtual void addStates();
            void startStateMachine();
            void handleEvents();

            void update(float deltaTime);
            void render(float deltaTime);
            inline void setEngine(Engine* engine) { rootNode.setEngine(engine); }

            StateMachine* getStateMachine() { return stateMachine.get(); }

            struct nk_context * getNuklearContext() { return nuklearContext; }
            void setNulkearContext(struct nk_context* nctx) { nuklearContext = nctx; }

            // Optional callbacks
            // NOTE: These are legacy from when Engine was abstract class, I need to refactor existing games
            // using these to use Scene Graphs or ECS and these custom callbacks can then be removed
            virtual void onAppPreUpdate(float deltaTime) {}
            virtual void onAppPostUpdate(float deltaTime) {}
            virtual void onAppRender(float deltaTime) {}
            virtual void onAppWindowResize(int newWidth, int newHeight) {}
            virtual void onAppImGuiRender(float deltaTime) {}
            virtual void onMouseMove(int mouseX, int mouseY) {}
            virtual void onMouseDown(int mouseX, int mouseY) {}
            virtual void onMouseUp() {}

        protected:
            void addToScene(SceneNode* child) { rootNode.addChild(child); }

            std::unique_ptr<StateMachine> stateMachine = nullptr; ///< state machine's instance
            //GameState* currentState = nullptr; ///< current state's instance
            IGameScreen* currentState = nullptr; ///< current state's instance

        private:
            Application(Application& app) {}
            void operator=(Application& app) {}

            // TODO: will hold root entity here
            // or maybe should go to the state?
            SceneNode rootNode;

            struct nk_context* nuklearContext;
    };
}

#endif // __APPLICATION__
