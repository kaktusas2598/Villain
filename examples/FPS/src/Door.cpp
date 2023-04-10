#include "Door.hpp"

#include "ResourceManager.hpp"

using namespace Villain;

const double TIME_TO_OPEN = 1.0f;
const double CLOSE_DELAY = 2.0f;

Door::Door(const glm::vec3& open) : MeshRenderer<VertexP1N1UV>(nullptr, Material()) {

    std::vector<Texture *> textures = {ResourceManager::Instance()->loadTexture(
            "assets/textures/WolfCollection.png", "door")};
    material = Material{"door", textures, 8};
    std::vector<VertexP1N1UV> vertices;

    // NOTE: Now Mesh class only needs vertices and indices and it could contain methods to build common primitives like these Also need an easy way to generate UV coords in 3D, and calculate normals vertices.push_back({glm::vec3(Start, Start, Start), glm::vec3(0.0f), glm::vec2(0.5f, 0.f)}); vertices.push_back({glm::vec3(Start, Height, Start), glm::vec3(0.0f), glm::vec2(0.5f, 0.25f)});
    vertices.push_back({glm::vec3(Length, Height, Start), glm::vec3(0.0f), glm::vec2(0.75f, 0.25f)});
    vertices.push_back({glm::vec3(Length, Start, Start), glm::vec3(0.0f), glm::vec2(0.75f, 0.f)});

    vertices.push_back({glm::vec3(Start, Start, Start), glm::vec3(0.0f), glm::vec2(0.73f, 0.f)});
    vertices.push_back({glm::vec3(Start, Height, Start), glm::vec3(0.0f), glm::vec2(0.73f, 0.25f)});
    vertices.push_back({glm::vec3(Start, Height, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.25f)});
    vertices.push_back({glm::vec3(Start, Start, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.f)});

    vertices.push_back({glm::vec3(Start, Start, Width), glm::vec3(0.0f), glm::vec2(0.5f, 0.f)});
    vertices.push_back({glm::vec3(Start, Height, Width), glm::vec3(0.0f), glm::vec2(0.5f, 0.25f)});
    vertices.push_back({glm::vec3(Length, Height, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.25f)});
    vertices.push_back({glm::vec3(Length, Start, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.f)});

    vertices.push_back({glm::vec3(Length, Start, Start), glm::vec3(0.0f), glm::vec2(0.73f, 0.f)});
    vertices.push_back({glm::vec3(Length, Height, Start), glm::vec3(0.0f), glm::vec2(0.73f, 0.25f)});
    vertices.push_back({glm::vec3(Length, Height, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.25f)});
    vertices.push_back({glm::vec3(Length, Start, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.f)});


    std::vector<unsigned int> indices{0, 1, 2,
        0, 2, 3,

        6, 5, 4,
        7, 6, 4,

        10, 9, 8,
        11, 10, 8,

        12, 13, 14,
        12, 14, 15,

    };

    mesh = new Mesh<VertexP1N1UV>(vertices, indices);
    // FIXME: segfault??? for now we set close pos in separate setter
    //closePosition = GetTransform()->getPos();
    openPosition = open;
}

void Door::update(float deltaTime) {
    // Animate doors opening/closing using animation keyframe times (in s) and linear interpolation (glm::mix)
    if (opening) {
        double time = animTimer.readSeconds();
        if (time < openTime) {
            float lerpFactor = (time - openingStartTime) / TIME_TO_OPEN;
            GetTransform()->setPos(glm::mix(closePosition, openPosition, lerpFactor));
        } else if (time < closingStartTime) {
            GetTransform()->setPos(openPosition);
        } else if (time < closeTime) {
            float lerpFactor = (time - closingStartTime) / TIME_TO_OPEN;
            GetTransform()->setPos(glm::mix(openPosition, closePosition, lerpFactor));
        } else {
            GetTransform()->setPos(closePosition);
            opening = false;
        }
    }
}

void Door::open() {
    if (opening)
        return;

    // If door starts opening/closing, calculate anim keyframes
    openingStartTime = animTimer.readSeconds();
    openTime = openingStartTime + TIME_TO_OPEN;
    closingStartTime = openTime + CLOSE_DELAY;
    closeTime = closingStartTime + TIME_TO_OPEN;

    opening = true;
}

glm::vec2 Door::getSize() {
    // Calculate correct collision size based on rotation
    if (GetTransform()->getEulerRot().y > 0) {
        return glm::vec2(Width, Length);
    } else {
        return glm::vec2(Length, Width);
    }
}
