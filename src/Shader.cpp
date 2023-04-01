#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "ErrorHandler.hpp"
#include "RendereringEngine.hpp"

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

    Shader::Shader(const std::string& vertexFile, const std::string& fragmentFile, const std::string geometryFile) {
        std::ifstream vShaderFile;
        std::ifstream gShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.open(vertexFile);
        gShaderFile.open(geometryFile);
        fShaderFile.open(fragmentFile);

        std::stringstream vShaderStream, fShaderStream, gShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        gShaderFile.close();
        fShaderFile.close();

        rendererID = createShader(vShaderStream.str(), gShaderStream.str(), fShaderStream.str());
    }


    Shader::~Shader() {
        GLCall(glDeleteProgram(rendererID));
    }

    void Shader::createFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
        rendererID = createShader(vertexSource, fragmentSource);
    }

    void Shader::createFromSource(const std::string& source) {
        // NOTE: this func is too similar to parseShader() which uses filestream instead of istringstream
        std::istringstream iss(source);
        enum class ShaderType {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        //while(getline(fs, line)) {
        for (std::string line; std::getline(iss, line); ) {
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

        rendererID = createShader(ss[0].str(), ss[1].str());
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

    unsigned int Shader::createShader(
            const std::string& vertexShader,
            const std::string& geometryShader,
            const std::string& fragmentShader) {
        GLCall(unsigned int program = glCreateProgram());

        unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int gs = compileShader(GL_GEOMETRY_SHADER, geometryShader);
        unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

        GLCall(glAttachShader(program, vs));
        GLCall(glAttachShader(program, gs));
        GLCall(glAttachShader(program, fs));
        GLCall(glLinkProgram(program));
        GLCall(glValidateProgram(program));

        // Delete intermediate shaders once they are linked to program
        GLCall(glDeleteShader(vs));
        GLCall(glDeleteShader(gs));
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

    void Shader::updateUniforms(Transform& transform, Material& material, RenderingEngine& renderingEngine, Camera& camera) {
        this->bind();
        // Step projection uniforms
        this->setUniformMat4f("model", transform.getTransformMatrix());
        this->setUniformMat4f("view", camera.getViewMatrix());
        this->setUniformMat4f("projection", camera.getProjMatrix());

        // TODO: sort material uniforms
        // Step material uniforms, other parameters like diffuse color,
        // diffuse and specular maps are set in Model class
        this->setUniform1f("material.shininess", material.getSpecularFactor());

        //unsigned int diffuseIndex = 1;
        //unsigned int specularIndex = 1;
        //for (unsigned int i = 0; i < Textures.size(); i++) {
            //Textures[i]->bind(i);
            //std::string slot;
            //std::string type = Textures[i]->getType();
            //if (type == "texture_diffuse")
                //slot = std::to_string(diffuseIndex++);
            //else if (type == "texture_specular")
                //slot = std::to_string(specularIndex++);

            //// TODO: need to check if color set when loading mesh and pass color as uniform here as well
            //this->setUniform1i("material." + type + slot, i);
        //}
        //this->setUniformVec4("material.diffuseColor", material.getDiffuseColor());
        material.getDiffuseMap()->bind();
        if (material.getSpecularMap() != nullptr)
            material.getSpecularMap()->bind(1);
        this->setUniform1i("material.texture_diffuse1", 0);
        this->setUniform1i("material.texture_specular1", 1);


        // Step light uniforms
        //this->setDirectionalLightUniforms("dirLight", dirLight);
        //this->setPointLightUniforms("pointLight", pointLight);
        //this->setSpotLightUniforms("spotLight", spotLight);

        // For lighting calculations
        this->setUniformVec3("viewPosition", camera.getPosition());

        // Forward Ambient shader uniforms, NOTE: move these elsewhere TEMPORARY
        this->setUniformVec3("color", glm::vec3(0.8f, 0.6f, 0.2f));
        this->setUniform1i("texture", 0);

    }

    void Shader::setDirectionalLightUniforms(const std::string& name, DirectionalLight dirLight) {
        setUniformVec3(name + ".direction", dirLight.Direction);
        setUniformVec3(name + ".base.ambient", dirLight.Base.AmbientColor);
        setUniformVec3(name + ".base.diffuse", dirLight.Base.DiffuseColor);
        setUniformVec3(name + ".base.specular", dirLight.Base.SpecularColor);
    }

    void Shader::setPointLightUniforms(const std::string& name, PointLight pointLight) {
        setUniformVec3(name + ".position", pointLight.Position);
        setUniformVec3(name + ".base.ambient", pointLight.Base.AmbientColor);
        setUniformVec3(name + ".base.diffuse", pointLight.Base.DiffuseColor);
        setUniformVec3(name + ".base.specular", pointLight.Base.SpecularColor);
        setUniform1f(name + ".constant", pointLight.Constant);
        setUniform1f(name + ".linear", pointLight.Linear);
        setUniform1f(name + ".quadratic", pointLight.Quadratic);
    }

    void Shader::setSpotLightUniforms(const std::string& name, SpotLight spotLight) {
        setUniformVec3(name + ".position", spotLight.Position);
        setUniformVec3(name + ".direction", spotLight.Direction);
        setUniform1f(name + ".cutOff", spotLight.CutOff);
        setUniform1f(name + ".outerCutOff", spotLight.OuterCutOff);
        setUniformVec3(name + ".base.ambient", spotLight.Base.AmbientColor);
        setUniformVec3(name + ".base.diffuse", spotLight.Base.DiffuseColor);
        setUniformVec3(name + ".base.specular", spotLight.Base.SpecularColor);
    }

}
