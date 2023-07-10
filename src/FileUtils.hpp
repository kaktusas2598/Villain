#pragma once

#include <cmrc/cmrc.hpp>
#include <string>

namespace Villain {

    // For common filesystem and file/resource operations
    class FileUtils {
        public:
            static std::string loadFile(const std::string& filePath);
            static char* loadBinaryFile(const std::string& filePath, int& size);
            static cmrc::file loadResource(const std::string& filePath);

            static bool fileExists(const std::string& filePath);
            static bool resourceExists(const std::string& filePath);
    };
}
