#pragma once

#include <string>

// Texture representing 3D Doom/Wolfenstein-like map
class Bitmap {
    public:
        Bitmap(const std::string& fileName);
        Bitmap(int w, int h): width(w), height(h) {}
        ~Bitmap();

        inline int getWidth() const { return width; }
        inline int getHeight() const { return height; }
        unsigned char* getPixel(int x, int y);
    private:
        int width, height, BPP;
        unsigned char* data;
};
