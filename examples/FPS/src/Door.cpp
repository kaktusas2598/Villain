#include "Door.hpp"

#include "ResourceManager.hpp"

using namespace Villain;

Door::Door() :
    MeshRenderer<VertexP1N1UV>(nullptr, Material()) {

        std::vector<Texture *> textures = {ResourceManager::Instance()->loadTexture(
                "assets/textures/WolfCollection.png", "door")};
        material = Material{"door", textures, 8};
        std::vector<VertexP1N1UV> vertices;

        // NOTE: Now Mesh class only needs vertices and indices and it could contain methods to build common primitives like these
        // Also need an easy way to generate UV coords in 3D, and calculate normals
        vertices.push_back({glm::vec3(Start, Start, Start), glm::vec3(0.0f), glm::vec2(0.5f, 1.f)});
        vertices.push_back({glm::vec3(Start, Height, Start), glm::vec3(0.0f), glm::vec2(0.5f, 0.75f)});
        vertices.push_back({glm::vec3(Length, Height, Start), glm::vec3(0.0f), glm::vec2(0.75f, 0.75f)});
        vertices.push_back({glm::vec3(Length, Start, Start), glm::vec3(0.0f), glm::vec2(0.75f, 1.f)});

        vertices.push_back({glm::vec3(Start, Start, Start), glm::vec3(0.0f), glm::vec2(0.73f, 1.f)});
        vertices.push_back({glm::vec3(Start, Height, Start), glm::vec3(0.0f), glm::vec2(0.73f, 0.75f)});
        vertices.push_back({glm::vec3(Start, Height, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.75f)});
        vertices.push_back({glm::vec3(Start, Start, Width), glm::vec3(0.0f), glm::vec2(0.75f, 1.f)});

        vertices.push_back({glm::vec3(Start, Start, Width), glm::vec3(0.0f), glm::vec2(0.5f, 1.f)});
        vertices.push_back({glm::vec3(Start, Height, Width), glm::vec3(0.0f), glm::vec2(0.5f, 0.75f)});
        vertices.push_back({glm::vec3(Length, Height, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.75f)});
        vertices.push_back({glm::vec3(Length, Start, Width), glm::vec3(0.0f), glm::vec2(0.75f, 1.f)});

        vertices.push_back({glm::vec3(Length, Start, Start), glm::vec3(0.0f), glm::vec2(0.73f, 1.f)});
        vertices.push_back({glm::vec3(Length, Height, Start), glm::vec3(0.0f), glm::vec2(0.73f, 0.75f)});
        vertices.push_back({glm::vec3(Length, Height, Width), glm::vec3(0.0f), glm::vec2(0.75f, 0.75f)});
        vertices.push_back({glm::vec3(Length, Start, Width), glm::vec3(0.0f), glm::vec2(0.75f, 1.f)});


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
    }
