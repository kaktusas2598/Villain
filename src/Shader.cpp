#include "Shader.hpp"

#include <fstream>
#include <sstream>

#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include "RendereringEngine.hpp"

// For common shader resources
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(Villain);

namespace Villain {

    Shader::Shader(): rendererID(0) {}

    Shader::Shader(const std::string& fileName): rendererID(0) {
        ShaderProgramSource shaderSource = parseShader(fileName);
        rendererID = createShader(shaderSource.VertexSource, shaderSource.FragmentSource);
    }

    Shader::Shader(const std::string& vertexFile, const std::string& fragmentFile, const std::string geometryFile): rendererID(0) {
        std::ifstream vShaderFile;
        std::ifstream gShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.open(vertexFile);
        fShaderFile.open(fragmentFile);

        std::stringstream vShaderStream, fShaderStream, gShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        if (geometryFile != std::string()) {
            gShaderFile.open(geometryFile);
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();

            rendererID = createShader(vShaderStream.str(), gShaderStream.str(), fShaderStream.str());
        } else {
            ShaderProgramSource shaderSource = {vShaderStream.str(), fShaderStream.str()};
            rendererID = createShader(shaderSource.VertexSource, shaderSource.FragmentSource);
        }
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

        // TODO: All code related to reusable shader code will have to be merged in with parseShader()
        const std::string INCLUDE_DIRECTIVE = "#include";

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        for (std::string line; std::getline(iss, line); ) {
            if (line.find(INCLUDE_DIRECTIVE) != std::string::npos) {
                // process include directive
                std::string file = line.substr(line.find(' ') + 1, line.length());
                std::string incSource = this->loadFromResource("res/shaders/" + file);
                std::istringstream inceIss(incSource);
                std::string incLine;
                // Append included file to shader
                for (std::string incLine; std::getline(inceIss, incLine); ) {
                    // NOTE: #shader directive must come before #include directive, to ensure type is set here
                    ss[(int)type] << incLine << '\n';
                }
            } else if (line.find("#shader") != std::string::npos) {
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

    void Shader::createFromResource(const std::string& source) {
        std::string src = this->loadFromResource("res/shaders/" + source + ".glsl");
        this->createFromSource(src);
    }

    std::string Shader::loadFromResource(const std::string& path) {
        auto shaderFs = cmrc::Villain::get_filesystem();
        if (shaderFs.exists(path)) {
            auto faShaderFile = shaderFs.open(path);
            std::string faSource(faShaderFile.begin());
            return faSource;
        } else {
            std::stringstream ss;
            ss << "Shader resource at " << path << " not found";
            Logger::Instance()->error(ss.str().c_str());
        }
        return std::string();
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

            std::stringstream ss;
            ss << "Failed to compile shader: " << message;
            Logger::Instance()->warn(ss.str().c_str());

            GLCall(glDeleteShader(id));
            return 0;
        }

        return id;
    }

    // This is very basic way of loading shader with both vertex and fragment shaders being in one file
    ShaderProgramSource Shader::parseShader(const std::string& fileName) {
        std::ifstream fs(fileName);

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
        if (location == -1) {
            std::stringstream ss;
            ss << "Uniform '" << name << "' doesn't exist";
            Logger::Instance()->warn(ss.str().c_str());
        }

        uniformLocationCache[name] = location;
        return location;

    }

    void Shader::updateUniforms(Transform& transform, Material& material, RenderingEngine& renderingEngine, Camera& camera) {
        this->bind();
        // Step projection uniforms
        this->setUniformMat4f("model", transform.getTransformMatrix());
        this->setUniformMat4f("view", camera.getViewMatrix());
        this->setUniformMat4f("projection", camera.getProjMatrix());

        this->setMaterialUniforms(material);

        // Set light uniforms
        // NOTE: this is really not good due to having to dynamic_cast every time and check
        if (renderingEngine.getActiveLight() != nullptr) {
            if (renderingEngine.getActiveLight()->type() == "directional") {
                DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(renderingEngine.getActiveLight());
                this->setDirectionalLightUniforms("dirLight", *dirLight);
            } else if (renderingEngine.getActiveLight()->type() == "spot") {
                SpotLight* spotLight = dynamic_cast<SpotLight*>(renderingEngine.getActiveLight());
                this->setSpotLightUniforms("spotLight", *spotLight);
            } else if (renderingEngine.getActiveLight()->type() == "point") {
                PointLight* pointLight = dynamic_cast<PointLight*>(renderingEngine.getActiveLight());
                this->setPointLightUniforms("pointLight", *pointLight);
            }
        }

        // Camera/view/eye pos for lighting calculations
        this->setUniformVec3("viewPosition", camera.getPosition());
    }

    void Shader::setMaterialUniforms(Material& material) {
        unsigned int diffuseIndex = 1;
        unsigned int specularIndex = 1;
        unsigned int normalIndex = 1;
        unsigned int i = 0;

        for (auto& mat: material.getDiffuseMaps()) {
            mat->bind(i);
            std::string slot = std::to_string(diffuseIndex++);
            this->setUniform1i("material.texture_diffuse" + slot, i);
            i++;
        }
        for (auto& mat: material.getSpecularMaps()) {
            mat->bind(i);
            std::string slot = std::to_string(specularIndex++);
            this->setUniform1i("material.texture_specular" + slot, i);
            i++;
        }
        for (auto& mat: material.getNormalMaps()) {
            mat->bind(i);
            std::string slot = std::to_string(normalIndex++);
            this->setUniform1i("material.texture_normal" + slot, i);
            i++;
        }

        this->setUniform1f("material.shininess", material.getSpecularFactor());
        this->setUniformVec4("material.diffuseColor", material.getDiffuseColor());
    }

    void Shader::setDirectionalLightUniforms(const std::string& name, DirectionalLight& dirLight) {
        setUniformVec3(name + ".direction", dirLight.Direction);
        setUniformVec3(name + ".base.ambient", dirLight.AmbientColor);
        setUniformVec3(name + ".base.diffuse", dirLight.DiffuseColor);
        setUniformVec3(name + ".base.specular", dirLight.SpecularColor);
    }

    void Shader::setPointLightUniforms(const std::string& name, PointLight& pointLight) {
        setUniformVec3(name + ".position", pointLight.Position);
        setUniformVec3(name + ".base.ambient", pointLight.AmbientColor);
        setUniformVec3(name + ".base.diffuse", pointLight.DiffuseColor);
        setUniformVec3(name + ".base.specular", pointLight.SpecularColor);
        setUniform1f(name + ".constant", pointLight.Constant);
        setUniform1f(name + ".linear", pointLight.Linear);
        setUniform1f(name + ".quadratic", pointLight.Quadratic);
    }

    void Shader::setSpotLightUniforms(const std::string& name, SpotLight& spotLight) {
        setUniformVec3(name + ".position", spotLight.Position);
        setUniformVec3(name + ".direction", spotLight.Direction);
        setUniform1f(name + ".cutOff", spotLight.CutOff);
        setUniform1f(name + ".outerCutOff", spotLight.OuterCutOff);
        setUniformVec3(name + ".base.ambient", spotLight.AmbientColor);
        setUniformVec3(name + ".base.diffuse", spotLight.DiffuseColor);
        setUniformVec3(name + ".base.specular", spotLight.SpecularColor);
    }

}
