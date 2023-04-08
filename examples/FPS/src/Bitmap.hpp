#pragma once

#include <string>

struct Pixel {
    int R;
    int G;
    int B;
};

// Texture representing 3D Doom/Wolfenstein-like map
class Bitmap {
    public:
        Bitmap(const std::string& fileName);
        Bitmap(int w, int h): width(w), height(h) {}
        ~Bitmap();

        inline int getWidth() const { return width; }
        inline int getHeight() const { return height; }
        Pixel getPixel(int x, int y);
    private:
        int width, height, BPP;
        unsigned char* data;
};
