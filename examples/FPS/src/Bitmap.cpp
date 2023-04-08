#include "Bitmap.hpp"

#include "stb_image.h"
#include <iostream>

Bitmap::Bitmap(const std::string& fileName): width(0), height(0), BPP(0) {
    stbi_set_flip_vertically_on_load(1);

    // Made it work correctly by specifying desirec channels to 0, it seems to
    // set 3 channels, so no alpha, but that should be enough to use for maps
    data = stbi_load(fileName.c_str(), &width, &height, &BPP, 0);

    if (data != nullptr) {
        std::cout << "Bit map width: " << width << "\n";
        std::cout << "Bit map height: " << height << "\n";
        std::cout << "Channel count: " << BPP << "\n";
    }
}

Bitmap::~Bitmap() {
    if (data)
        stbi_image_free(data);
}

Pixel Bitmap::getPixel(int x, int y) {
    unsigned char* pixelPointer = data + (y + x * height) * BPP;
    Pixel pixel;
    pixel.R = (int)pixelPointer[0];
    pixel.G = (int)pixelPointer[1];
    pixel.B = (int)pixelPointer[2];

    return pixel;
}
