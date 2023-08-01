#include "Shader.hpp"

#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include "FileUtils.hpp"
#include "RendereringEngine.hpp"
#include <sstream>

namespace Villain {

    Shader::Shader(): rendererID(0) {}

    Shader::Shader(const std::string& fileName): rendererID(0) {
        Logger::Instance()->info("Loading shader: {}", fileName);
        createFromSource(FileUtils::loadFile(fileName));
    }

    Shader::Shader(const std::string& vertexFile, const std::string& fragmentFile, const std::string geometryFile): rendererID(0) {
        Logger::Instance()->info("Loading shaders: {}, {}", vertexFile, fragmentFile);
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
        bool geometryShaderIncluded = false;
        std::stringstream ss[3];
        ShaderType type = ShaderType::NONE;
        for (std::string line; std::getline(iss, line); ) {
            if (line.find(INCLUDE_DIRECTIVE) != std::string::npos) {
                //VILLAIN_ASSERT(type == ShaderType::NONE, "#shader directive must be set before using #include directive: {}", source);
                // Recursively include shader headers
                includeHeader(ss, type, line);
            } else if (line.find("#shader") != std::string::npos) {
                if (line.find("vertex") != std::string::npos) {
                    type = ShaderType::VERTEX;
                } else if (line.find("fragment") != std::string::npos) {
                    type = ShaderType::FRAGMENT;
                } else if (line.find("geometry") != std::string::npos) {
                    type = ShaderType::GEOMETRY;
                    geometryShaderIncluded = true;
                }
            } else {
                if (type != ShaderType::NONE)
                    ss[(int)type] << line << '\n';
            }
        }

        if (geometryShaderIncluded) {
            rendererID = createShader(ss[(int)ShaderType::VERTEX].str(), ss[(int)ShaderType::GEOMETRY].str(), ss[(int)ShaderType::FRAGMENT].str());
        } else {
            rendererID = createShader(ss[(int)ShaderType::VERTEX].str(), ss[(int)ShaderType::FRAGMENT].str());
        }
    }

    void Shader::includeHeader(std::stringstream* ss, ShaderType shaderType, const std::string& line) {
        // process include directive
        std::string file = line.substr(line.find(' ') + 1, line.length());
        std::string incSource = std::string(FileUtils::loadResource("res/shaders/" + file).begin());
        std::istringstream inceIss(incSource);
        std::string incLine;

        // Append included file to shader
        for (std::string incLine; std::getline(inceIss, incLine); ) {
            if (incLine.find("#include") != std::string::npos) {
                includeHeader(ss, shaderType, incLine);
            } else {
                ss[(int)shaderType] << incLine << '\n';
            }
        }
    }

    Shader* Shader::createFromResource(const std::string& source) {
        std::string src = std::string(FileUtils::loadResource("res/shaders/" + source + ".glsl").begin());
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

            Logger::Instance()->warn("Failed to compile shader: %s", message);

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
    void Shader::setUniform1ui(const std::string& name, unsigned int value) {
        GLCall(glUniform1ui(getUniformLocation(name), value));
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

    void Shader::updateUniforms(Transform& transform, RenderingEngine& renderingEngine, Camera& camera) {
        this->bind();
        // Step projection uniforms
        // TODO: possibly move this to Renderer class, Camera can be submitted in start() method and transform submitted to each draw
        this->setUniformMat4f("model", transform.getTransformMatrix());
        this->setUniformMat4f("view", camera.getViewMatrix());
        this->setUniformMat4f("projection", camera.getProjMatrix());

        // Set light uniforms
        if (renderingEngine.getActiveLight() != nullptr) {
            renderingEngine.getActiveLight()->setUniforms(*this);
        }

        // Camera/view/eye pos for lighting calculations
        this->setUniformVec3("viewPosition", camera.getPosition());
    }

    void Shader::setFogUniforms(RenderingEngine& renderingEngine, Camera& camera) {
        setUniformVec3("fogColor", *renderingEngine.getFogColor());
        setUniform1i("useExponentialFog", *renderingEngine.exponentialFogEnabled());
        // Exponential Fog Parameters
        setUniform1f("fogDensity", *renderingEngine.getFogDensity());
        setUniform1f("fogGradient", *renderingEngine.getFogGradient());
        // Layered Fog Parameters
        setUniform1f("layeredFogTop", *renderingEngine.getLayeredFogTop());
        setUniform1f("layeredFogEnd", *renderingEngine.getLayeredFogEnd());
        setUniformVec3("viewPosition", camera.getPosition());
    }
}
