#include "Application.hpp"

#include "IGameScreen.hpp"
#include "SceneParser.hpp"
#include "StateParser.hpp"

namespace Villain {

    Application::Application(): rootNode("rootNode"), stateMachine(std::make_unique<StateMachine>(this)) {}

    void Application::addStates() {
        Logger::Instance()->info("Adding states.");
        StateParser stateParser;
        std::vector<IGameScreen*> states;
        if (!stateParser.loadStates("state.xml", &states)) {
            Logger::Instance()->error("Failed adding states.");
        }
        for (auto& state: states) {
            stateMachine->addScreen(state);
        }
        if (states.size() > 0)
            stateMachine->setScreen(states[0]->getID());
        states.clear();

        return;
    }

    void Application::startStateMachine() {
        currentState = stateMachine->getCurrentScreen();

        if (currentState != nullptr) {
            //initialize game screen elements
            currentState->onEntry();
            //set the initial game screen to ScreenState::RUNNING
            currentState->setRunning();
        }
    }


    void Application::handleEvents(float deltaTime) {
        rootNode.handleInput(deltaTime);
    }

    void Application::update(float deltaTime) {
        // Update scene
        rootNode.update(deltaTime);

        // Update state
        if (currentState) {
            switch (currentState->getScreenState()) {
                //update the current running screen
                case ScreenState::RUNNING:
                    currentState->update(deltaTime);
                    break;
                    //change to next screen
                case ScreenState::CHANGE_NEXT:
                    //clean up running screen
                    currentState->onExit();
                    currentState = stateMachine->moveNext();
                    if (currentState) {
                        //initialize new running screen
                        currentState->setRunning();
                        currentState->onEntry();
                    }
                    break;
                    //change to previous screen
                case ScreenState::CHANGE_PREVIOUS:
                    //clean up running screen
                    currentState->onExit();
                    currentState = stateMachine->movePrev();
                    if (currentState) {
                        //initialize new running screen
                        currentState->setRunning();
                        currentState->onEntry();
                    }
                    break;
                    //exit game
                case ScreenState::EXIT_APPLICATION:
                    exit();
                    //engine->exit();
                    break;
                default:
                    break;
            }
        }
        //else { exit(); }

    }

    void Application::render(RenderingEngine* renderingEngine, float deltaTime) {
        //Render scene
        renderingEngine->render(&rootNode, deltaTime);

        // Render current state
        if (currentState && currentState->getScreenState() == ScreenState::RUNNING) {
            currentState->draw();
        }
    }

    void Application::postRenderPass(RenderingEngine* renderingEngine) {
        renderingEngine->postRender();
    }

    void Application::exit() {
        // NOTE: gotta be a better way
        Engine::setRunning(false);

        if (currentState != nullptr) {
            currentState->onExit();
        }

        if (stateMachine) {
            stateMachine->destroy();
            stateMachine.reset();
        }
    }

    void Application::deleteNode(SceneNode* node) {
        if (!node)
            return;

        // Remove the node from the root's children list
        auto& rootChildren = rootNode.getChildren();
        auto it = std::find(rootChildren.begin(), rootChildren.end(), node);
        if (it != rootChildren.end()) {
            rootChildren.erase(it);
        }

        // Delete the node and all its children (recursively)
        for (auto& child : node->getChildren()) {
            deleteNode(child);
        }

        delete node;
    }

    void Application::loadScene(const std::string& fileName) {
        SceneParser sceneParser;
        sceneParser.loadSceneGraph(fileName, &rootNode);
    }

    void Application::saveScene(const std::string& fileName) {
        SceneParser sceneParser;
        sceneParser.saveSceneGraph(fileName, &rootNode);
    }
}
