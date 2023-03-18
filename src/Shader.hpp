#ifndef __Shader__
#define __Shader__

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

namespace Villain {

    struct ShaderProgramSource {
        std::string VertexSource;
        std::string FragmentSource;
    };


    // This is really Shader Program, as it loads and compiles both vertex and fragment shaders
    class Shader {
        public:
            Shader(const std::string& fileName);
            Shader(const std::string& vertexFile, const std::string& fragmentFile);
            ~Shader();


            void bind() const;
            void unbind() const;

            unsigned int getRendererID() const { return rendererID; }

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
            unsigned int compileShader(unsigned int type, const std::string& source);
            ShaderProgramSource parseShader(const std::string& fileName);

            int getUniformLocation(const std::string& name);
    };

}

#endif // __Shader__
