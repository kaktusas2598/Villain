#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

// Include the necessary header for the epsilon comparison
#include <glm/gtc/epsilon.hpp>

#include "SceneNode.hpp"
#include "NodeComponent.hpp"

// TODO: Possibly mock this instead of creating sample component class for testing
class TestComponent : public Villain::NodeComponent {
public:
    void handleInput(float deltaTime) override { InputInvoked = true; }
    void update(float deltaTime) override { UpdateInvoked = true; }
    void render(Villain::Shader& shader, Villain::RenderingEngine& renderingEngine, Villain::Camera& camera) override { RenderInvoked = true; }
    void addToEngine(Villain::Engine* engine) override { AddedToEngine = true; }

    bool InputInvoked = false;
    bool UpdateInvoked = false;
    bool RenderInvoked = false;
    bool AddedToEngine = false;
};

// FIXME: Doubt the issue is with SceneNode class, test case cuasing segfault, might be related to dependencies,
// starting happening after introducing render ordering in SceneNode::render()
TEST_CASE("SceneNode Test", "[Scene Graph]") {
    SECTION("Create and modify SceneNode") {
        Villain::SceneNode node("Node1");

        REQUIRE(node.getName() == "Node1");
        REQUIRE(node.getChildren().empty());
        REQUIRE(node.getComponents().empty());

        node.setName("ModifiedNode");
        REQUIRE(node.getName() == "ModifiedNode");

        TestComponent* component = new TestComponent();
        node.addComponent(component);
        REQUIRE(node.getComponents().size() == 1);
        REQUIRE(node.getComponents()[0] == component);


        //Villain::SceneNode childNode("ChildNode");
        //node.addChild(&childNode);
        //REQUIRE(node.getChildren().size() == 1);
        //REQUIRE(node.getChildren()[0] == &childNode);

        //TestComponent* childComponent = new TestComponent();
        //childNode.addComponent(childComponent);

        node.handleInput(0.0f);
        REQUIRE(component->InputInvoked);
        //REQUIRE(childComponent->InputInvoked);

        node.update(0.0f);
        REQUIRE(component->UpdateInvoked);
        //REQUIRE(childComponent->UpdateInvoked);

        //node.render(nullptr, nullptr, nullptr);
        //REQUIRE(component->RenderInvoked);
        //REQUIRE(childComponent->RenderInvoked);

        //// TODO: mock Engine
        ////node.setEngine()

        //REQUIRE(node.findByID(1) == &childNode);
        //REQUIRE(node.findByID(100) == nullptr);
        //REQUIRE(childNode.getID() == 1);

        //node.removeChild(&childNode);
        //REQUIRE(node.getChildren().empty());

        //REQUIRE_FALSE(node.isSelected());
        //node.setSelected(true);
        //REQUIRE(node.isSelected());
    }
}

// TODO: Investigate why memory seems to be cleared in SceneNode destructor, but
// children and components in this test case still seem to point to some memory after delete is called
//TEST_CASE("SceneNode Destructor Test", "[Scene Graph]") {
    //SECTION("Memory Deallocation") {
        //Villain::SceneNode* rootNode = new Villain::SceneNode("RootNode");
        //Villain::SceneNode* childNode1 = new Villain::SceneNode("ChildNode1");
        //Villain::SceneNode* childNode2 = new Villain::SceneNode("ChildNode2");

        //TestComponent* component1 = new TestComponent();
        //TestComponent* component2 = new TestComponent();

        //rootNode->addComponent(component1);
        //rootNode->addChild(childNode1);
        //rootNode->addChild(childNode2);

        //childNode1->addComponent(component2);

        //// Check if the components and child nodes are present before destruction
        //REQUIRE(rootNode->getComponents().size() == 1);
        //REQUIRE(rootNode->getChildren().size() == 2);
        //REQUIRE(childNode1->getComponents().size() == 1);

        //// Printfing memory address of component pointer
        //printf("%p\n", (void*)&component1);
        //// Destroy the root node
        //delete rootNode;
        //rootNode = nullptr;


        //// Check if the memory has been deallocated
        //// After destruction, accessing the deleted objects will result in undefined behavior, so it's important to check for null pointers
        //REQUIRE(rootNode == nullptr);
        //REQUIRE(component1 == nullptr);
        ////REQUIRE(component2 == nullptr);
        ////REQUIRE(childNode1 == nullptr);
        ////REQUIRE(childNode2 == nullptr);
    //}
//}
