#include "FileUtils.hpp"

#include "Logger.hpp"
#include <sstream>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(Villain);

// For stat() to get file size, not tested on Windows
#include <sys/stat.h>

namespace Villain {

    std::string FileUtils::loadFile(const std::string& filePath) {
        std::ifstream file;
        file.open(filePath);
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();
        return ss.str();
    }

    // NOTE: could rewrite this in C++?
    char* FileUtils::loadBinaryFile(const std::string& filePath, int& size) {
        FILE* file = fopen(filePath.c_str(), "rb");

        if (!file) {
            std::stringstream ss;
            ss << "Error opening " << filePath;
            Logger::Instance()->error(ss.str().c_str());
        }

        struct stat fileData;
        if (stat(filePath.c_str(), &fileData) != 0) {
            Logger::Instance()->error("Error occured trying to stat binary file.");
        }

        size = fileData.st_size;
        char* buffer = (char*)malloc(size);
        assert(p);

        size_t bytesRead = fread(buffer, 1, size, file);

        if (bytesRead != size) {
            Logger::Instance()->error("Error occured trying to read binary file.");
            return nullptr;
        }

        fclose(file);
        return buffer;
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
