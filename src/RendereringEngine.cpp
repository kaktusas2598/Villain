#include "RendereringEngine.hpp"

#include "Camera2D.hpp"
#include "Camera3D.hpp"
#include "Engine.hpp"
#include "InputManager.hpp"
#include "Mesh.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"

// For common shader resources
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(Villain);

namespace Villain {

    RenderingEngine::RenderingEngine() {

        //NOTE: temporary!
        defaultShader = new Shader();

        //// CMRC Resource usage
        auto shaderFs = cmrc::Villain::get_filesystem();
        if (shaderFs.exists("res/shaders/forward-ambient.glsl")) {
            auto faShaderFile = shaderFs.open("res/shaders/forward-ambient.glsl");
            std::string faSource(faShaderFile.begin());
            defaultShader->createFromSource(faSource);
        }
        mainCamera = new Camera3D();
    }

    void RenderingEngine::render(SceneNode* node) {
        node->render(defaultShader, this, mainCamera);


        // Loop through lights now
        // Render all lights, generate shadow maps, etc

    }
}
