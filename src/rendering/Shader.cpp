#include "Shader.hpp"

#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include "FileUtils.hpp"
#include "RendereringEngine.hpp"
#include <sstream>

namespace Villain {

    Shader::Shader(): rendererID(0) {}

    Shader::Shader(const std::string& fileName): rendererID(0) {
        createFromSource(FileUtils::loadFile(fileName));
    }

    Shader::Shader(const std::string& vertexFile, const std::string& fragmentFile, const std::string geometryFile): rendererID(0) {
        std::string vertexShader = FileUtils::loadFile(vertexFile);
        std::string fragmentShader = FileUtils::loadFile(fragmentFile);

        if (geometryFile != std::string()) {
            std::string geometryShader = FileUtils::loadFile(geometryFile);

            rendererID = createShader(vertexShader, fragmentShader, geometryShader);
        } else {
            rendererID = createShader(vertexShader, fragmentShader);
        }
    }

    Shader::~Shader() {
        GLCall(glDeleteProgram(rendererID));
    }

    void Shader::createFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
        rendererID = createShader(vertexSource, fragmentSource);
    }

    void Shader::createFromSource(const std::string& source) {
        std::istringstream iss(source);
        const std::string INCLUDE_DIRECTIVE = "#include";

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        for (std::string line; std::getline(iss, line); ) {
            if (line.find(INCLUDE_DIRECTIVE) != std::string::npos) {
                // process include directive
                std::string file = line.substr(line.find(' ') + 1, line.length());
                std::string incSource = FileUtils::loadResource("res/shaders/" + file);
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

    Shader* Shader::createFromResource(const std::string& source) {
        std::string src = FileUtils::loadResource("res/shaders/" + source + ".glsl");
        Shader* shader = new Shader();
        shader->createFromSource(src);
        return shader;
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
        unsigned int diffuseSlot = 0;
        unsigned int specularSlot = 1;
        unsigned int normalSlot = 2;
        unsigned int displacementSlot = 3;
        // Base color/diffuse map
        if (material.getDiffuseMap() == nullptr) {
            this->setUniform1i("material.useDiffuseMap", 0);
        } else {
            this->setUniform1i("material.useDiffuseMap", 1);
            material.getDiffuseMap()->bind(diffuseSlot);
            this->setUniform1i("material.texture_diffuse", diffuseSlot);
        }
        // Specular map
        if (material.getSpecularMap() == nullptr) {
            this->setUniform1i("material.useSpecularMap", 0);
        } else {
            this->setUniform1i("material.useSpecularMap", 1);
            material.getSpecularMap()->bind(specularSlot);
            this->setUniform1i("material.texture_specular", specularSlot);
        }
        // Normal/bump map
        if (material.getNormalMap() == nullptr) {
            this->setUniform1i("material.useNormalMap", 0);
        } else {
            this->setUniform1i("material.useNormalMap", 1);
            material.getNormalMap()->bind(normalSlot);
            this->setUniform1i("material.texture_normal", normalSlot);
        }
        // Parallax displacement map
        if (material.getDislacementMap() == nullptr) {
            this->setUniform1i("material.useDispMap", 0);
        } else {
            this->setUniform1i("material.useDispMap", 1);
            material.getDislacementMap()->bind(displacementSlot);
            this->setUniform1i("material.texture_disp", displacementSlot);
            this->setUniform1f("material.dispMapScale", material.getDispMapScale());
            this->setUniform1f("material.dispMapBias", material.getDispMapBias());
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
