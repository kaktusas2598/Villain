#ifndef __Shader__
#define __Shader__

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

#include "Camera.hpp"
#include "../components/Light.hpp"
#include "Material.hpp"
#include "Transform.hpp"

namespace Villain {

    class RenderingEngine;

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
    };

    // This is really Shader Program, as it loads and compiles both vertex and fragment shaders
    class Shader {
        public:
            // Empty shader object, use createShader() to pass shader source directly
            Shader();
            // Create shader from a single file which has vertex and fragment shaders
            Shader(const std::string& fileName);
            // Create shader program from multiple files: vertex, fragment and optionally geometry shader
            Shader(const std::string& vertexFile, const std::string& fragmentFile, const std::string geometryFile = std::string());
            ~Shader();

            // Create shader from embedded resource library
            static Shader* createFromResource(const std::string& source);

            // To be used with empty constructor if we don't want to load shaders from files
            void createFromSource(const std::string& vertexSource, const std::string& fragmentSource);
            void createFromSource(const std::string& source);
            void includeHeader(std::stringstream* ss, ShaderType shaderType, const std::string& line);

            void bind() const;
            void unbind() const;

            unsigned int getRendererID() const { return rendererID; }

            // Set specific uniforms
            void setDirectionalLightUniforms(const std::string& name, DirectionalLight& dirLight);
            void setPointLightUniforms(const std::string& name, PointLight& pointLight);
            void setSpotLightUniforms(const std::string& name, SpotLight& spotLight);
            void setMaterialUniforms(Material& material);
            void setFogUniforms(RenderingEngine& renderingEngine, Camera& camera);
            // Update multiple default uniforms at once
            void updateUniforms(Transform& transform, Material& material, RenderingEngine& renderingEngine, Camera& camera);

            // Set uniforms, TODO: use templates to have multiple types of uniforms
            void setUniform1i(const std::string& name, int value);
            void setUniform1f(const std::string& name, float value);
            void setUniform3f(const std::string& name, float v0, float v1, float v2);
            void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
            void setUniformVec3(const std::string& name, const glm::vec3& vec);
            void setUniformVec4(const std::string& name, const glm::vec4& vec);
            void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

        private:
            unsigned int rendererID;
            // caching for uniforms
            std::unordered_map<std::string, int> uniformLocationCache;

            unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
            unsigned int createShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader);
            unsigned int compileShader(unsigned int type, const std::string& source);

            int getUniformLocation(const std::string& name);
    };
}

#endif // __Shader__
