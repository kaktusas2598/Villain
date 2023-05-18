#ifndef __FILE_UTILS__
#define __FILE_UTILS__

#include <string>

namespace Villain {

    // For common filesystem and file/resource operations
    // TODO: implement methods for filesystem library which can be used in imgui for editor
    class FileUtils {
        public:
            static std::string loadFile(const std::string& filePath);
            static char* loadBinaryFile(const std::string& filePath, int& size);
            static std::string loadResource(const std::string& filePath);
    };
}

#endif // __FILE_UTILS__
