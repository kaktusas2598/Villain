#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include <fstream>
#include "FileUtils.hpp"

TEST_CASE("FileUtils Tests") {
    SECTION("loadFile") {
        std::string filePath = "test.txt";
        std::string expectedContent = "This is a test file.";

        // Create a temporary test file with some content
        std::ofstream file(filePath);
        file << expectedContent;
        file.close();

        // Load the file using FileUtils
        std::string loadedContent = Villain::FileUtils::loadFile(filePath);

        // Check if the loaded content matches the expected content
        REQUIRE(loadedContent == expectedContent);

        // Clean up the test file
        std::remove(filePath.c_str());
    }

    SECTION("loadBinaryFile") {
        std::string filePath = "test.bin";
        int expectedSize = 10;
        char* expectedBuffer = new char[expectedSize];

        // Create a temporary test binary file with some data
        FILE* file = fopen(filePath.c_str(), "wb");
        fwrite(expectedBuffer, 1, expectedSize, file);
        fclose(file);

        // Load the binary file using FileUtils
        int loadedSize = 0;
        char* loadedBuffer = Villain::FileUtils::loadBinaryFile(filePath, loadedSize);

        // Check if the loaded size matches the expected size
        REQUIRE(loadedSize == expectedSize);

        // Check if the loaded buffer matches the expected buffer
        REQUIRE(memcmp(loadedBuffer, expectedBuffer, expectedSize) == 0);

        // Clean up the test file and buffer
        std::remove(filePath.c_str());
        delete[] expectedBuffer;
        free(loadedBuffer);
    }

    SECTION("Non existant binary file") {
        int loadedSize = 0;
        char* loadedContent = Villain::FileUtils::loadBinaryFile("404.bin", loadedSize);

        REQUIRE(loadedContent == nullptr);
    }

    SECTION("Load non existant resource") {
        // Load the resource file using FileUtils
        std::string loadedContent = Villain::FileUtils::loadResource("test");

        // Check if the loaded content matches the expected content
        REQUIRE(loadedContent == std::string());
    }
}
