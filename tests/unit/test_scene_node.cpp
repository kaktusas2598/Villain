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
    void addToEngine(Villain::Engine* engine) override {}

    bool InputInvoked = false;
    bool UpdateInvoked = false;
    bool RenderInvoked = false;
};

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


        Villain::SceneNode childNode("ChildNode");
        node.addChild(&childNode);
        REQUIRE(node.getChildren().size() == 1);
        REQUIRE(node.getChildren()[0] == &childNode);

        TestComponent* childComponent = new TestComponent();
        childNode.addComponent(childComponent);

        node.handleInput(0.0f);
        REQUIRE(component->InputInvoked);
        REQUIRE(childComponent->InputInvoked);

        node.update(0.0f);
        REQUIRE(component->UpdateInvoked);
        REQUIRE(childComponent->UpdateInvoked);

        node.render(nullptr, nullptr, nullptr);
        REQUIRE(component->RenderInvoked);
        REQUIRE(childComponent->RenderInvoked);

        REQUIRE(node.findByID(1) == &childNode);
        REQUIRE(node.findByID(100) == nullptr);
        REQUIRE(childNode.getID() == 1);

        node.removeChild(&childNode);
        REQUIRE(node.getChildren().empty());

        REQUIRE_FALSE(node.isSelected());
        node.setSelected(true);
        REQUIRE(node.isSelected());
    }
}
