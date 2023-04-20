#include "FileUtils.hpp"

#include "Logger.hpp"
#include <sstream>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(Villain);

namespace Villain {

    std::string FileUtils::loadFile(const std::string& filePath) {
        std::ifstream file;
        file.open(filePath);
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();
        return ss.str();
    }

    std::string FileUtils::loadResource(const std::string& filePath) {
        auto resourceFs = cmrc::Villain::get_filesystem();
        if (resourceFs.exists(filePath)) {
            auto resourceFile = resourceFs.open(filePath);
            std::string source(resourceFile.begin());
            return source;
        } else {
            std::stringstream ss;
            ss << "Resource at " << filePath << " not found";
            Logger::Instance()->error(ss.str().c_str());
        }
        return std::string();
    }
}
