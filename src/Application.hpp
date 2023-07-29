#pragma once

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
            void exit();

            void handleEvents(float deltaTime);
            void update(float deltaTime);
            void render(RenderingEngine* RenderingEngine, float deltaTime);
            // HACK: used for now to make sure stuff not handled by RenderingEngine gets drawn to custom framebuffer
            void postRenderPass(RenderingEngine* RenderingEngine);
            inline void setEngine(Engine* engine) { rootNode.setEngine(engine); }

            StateMachine* getStateMachine() { return stateMachine.get(); }
            SceneNode* getRootNode() { return &rootNode; }
            void deleteNode(SceneNode* node);
            void loadScene(const std::string& fileName);

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

            // 2023-04-09: Was protected before, but had to expose this class to 3D level in FPS project
            void addToScene(SceneNode* child) { rootNode.addChild(child); }
        protected:

            std::unique_ptr<StateMachine> stateMachine = nullptr; ///< state machine's instance
            //GameState* currentState = nullptr; ///< current state's instance
            IGameScreen* currentState = nullptr; ///< current state's instance

        private:
            Application(Application& app): rootNode("rootNode") {}
            void operator=(Application& app) {}

            SceneNode rootNode;

            // NOTE: problably should be in different class, maybe need to create UI class to abstract nuklear stuff
            struct nk_context* nuklearContext;
    };
}
