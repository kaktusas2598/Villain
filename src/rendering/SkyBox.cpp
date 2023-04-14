#include "SkyBox.hpp"

#include "VertexBufferLayout.hpp"
#include "Renderer.hpp"

namespace Villain {

    SkyBox::SkyBox(const std::vector<std::string>& faces, const std::string& cubemapShaderPath) {

        float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,

            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
        };

        unsigned int indices[] = {
            // FRONT
            0, 1, 2,
            2, 3, 0,
            // BACK
            4, 5, 6,
            6, 7, 4,
            // UP
            8, 9, 10,
            10, 11, 8,
            // DOWN
            12, 13, 14,
            14, 15, 12,
            // LEFT
            16, 17, 18,
            18, 19, 16,
            // RIGHT
            20, 21, 22,
            22, 23, 20
        };


        skyboxVao = std::make_unique<VertexArray>();
        skyboxVbo = std::make_unique<VertexBuffer>(skyboxVertices, 24 * 3 * sizeof(float));

        VertexBufferLayout skyboxLayout;
        skyboxLayout.push<float>(3);

        skyboxVao->addBuffer(*skyboxVbo, skyboxLayout);

        // Generate and bind index buffer object
        skyboxIbo = std::make_unique<IndexBuffer>(indices, 36);

        cubemapTexture = new Texture(faces);
        cubemapShader = new Shader(cubemapShaderPath);

        cubemapShader->bind();
        cubemapShader->setUniform1i("cubemap", 0);
    }

    SkyBox::~SkyBox() {
    }

    void SkyBox::render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
        Renderer renderer;
        cubemapTexture->bind();
        // Render skybox
        // Could be drawn before everything else, but that would be much slower because all of it would be rendered
        // We render skybox last and set depth func to equal because in skybox shader we will set depth calues to Z
        // and skybox fragments wont be drawn if depth is less than 1.0 meaning there are objects in front of the skybox
        glDepthFunc(GL_EQUAL);
        cubemapShader->bind();
        cubemapShader->setUniformMat4f("projection", projectionMatrix);
        glm::mat4 skyboxViewMat = glm::mat4(glm::mat3(viewMatrix));
        cubemapShader->setUniformMat4f("view", skyboxViewMat);
        cubemapTexture->bind();
        renderer.draw(*skyboxVao, *skyboxIbo, *cubemapShader);
        glDepthFunc(GL_LESS); // Back to default
    }

}
