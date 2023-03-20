#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "ErrorHandler.hpp"

namespace Villain {

    Shader::Shader(): rendererID(0) {}

    Shader::Shader(const std::string& fileName): rendererID(0) {
        ShaderProgramSource shaderSource = parseShader(fileName);
        rendererID = createShader(shaderSource.VertexSource, shaderSource.FragmentSource);
    }

    Shader::Shader(const std::string& vertexFile, const std::string& fragmentFile): rendererID(0) {
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.open(vertexFile);
        fShaderFile.open(fragmentFile);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();


        ShaderProgramSource shaderSource = {vShaderStream.str(), fShaderStream.str()};
        rendererID = createShader(shaderSource.VertexSource, shaderSource.FragmentSource);
    }


    Shader::~Shader() {
        GLCall(glDeleteProgram(rendererID));
    }

    unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {

        GLCall(unsigned int program = glCreateProgram());

        unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

        GLCall(glAttachShader(program, vs));
        GLCall(glAttachShader(program, fs));
        GLCall(glLinkProgram(program));
        GLCall(glValidateProgram(program));

        // Delete intermediate shaders once they are linked to program
        GLCall(glDeleteShader(vs));
        GLCall(glDeleteShader(fs));

        return program;
    }

    unsigned int Shader::compileShader(unsigned int type, const std::string& source) {

        GLCall(unsigned int id = glCreateShader(type));
        const char* src = source.c_str();

        GLCall(glShaderSource(id, 1, &src, nullptr));

        GLCall(glCompileShader(id));

        // shader error handling
        int result;
        GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE) {
            int length;
            GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
            // Use alloca() to allocate error message buffer on the stack
            char * message = (char*)alloca(length * sizeof(char));
            GLCall(glGetShaderInfoLog(id, length, &length, message));
            std::cout << "Failed to compile shader: " << message << std::endl;
            GLCall(glDeleteShader(id));
            return 0;
        }

        return id;
    }

    // This is very basic way of loading shader with both vertex and fragment shaders being in one file
    ShaderProgramSource Shader::parseShader(const std::string& fileName) {
        std::ifstream fs(fileName);

        enum class ShaderType {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        while(getline(fs, line)) {
            if (line.find("#shader") != std::string::npos) {
                if (line.find("vertex") != std::string::npos) {
                    type = ShaderType::VERTEX;

                } else if (line.find("fragment") != std::string::npos) {
                    type = ShaderType::FRAGMENT;

                }
            } else {
                if (type != ShaderType::NONE)
                    ss[(int)type] << line << '\n';
            }
        }

        return {ss[0].str(), ss[1].str()};
    }



    void Shader::bind() const {
        GLCall(glUseProgram(rendererID));
    }

    void Shader::unbind() const {
        GLCall(glUseProgram(0));
    }

    void Shader::setUniform1i(const std::string& name, int value) {
        GLCall(glUniform1i(getUniformLocation(name),value));
    }
    void Shader::setUniform1f(const std::string& name, float value) {
        GLCall(glUniform1f(getUniformLocation(name), value));
    }

    void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2) {
        GLCall(glUniform3f(getUniformLocation(name), v0, v1, v2));
    }

    void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
        GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
    }

    void Shader::setUniformVec3(const std::string& name, const glm::vec3& vec) {
        GLCall(glUniform3f(getUniformLocation(name), vec.x, vec.y, vec.z));
    }

    void Shader::setUniformVec4(const std::string& name, const glm::vec4& vec) {
        GLCall(glUniform4f(getUniformLocation(name), vec.x, vec.y, vec.z, vec.w));
    }

    void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
        GLCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
    }

    int Shader::getUniformLocation(const std::string& name) {
        if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
            return uniformLocationCache[name];
        }
        GLCall(int location = glGetUniformLocation(rendererID, name.c_str()));
        if (location == -1)
            std::cout << "Uniform '" << name << "' doesn't exist!" << std::endl;

        uniformLocationCache[name] = location;
        return location;

    }
}
