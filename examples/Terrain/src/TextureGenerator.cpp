#include "TextureGenerator.hpp"

#include "ErrorHandler.hpp"
#include "ResourceManager.hpp"
#include "Terrain.hpp"

void TextureGenerator::loadTile(const char* fileName) {
    if (numTextureTiles == MAX_TEXTURE_TILES)
        exitWithError("Exceeded maximum number of allowed terrain texture tiles.");

    textureTiles[numTextureTiles].Tex = Villain::ResourceManager::Instance()->loadTexture(fileName, "TT " + std::to_string(numTextureTiles));
    numTextureTiles++;
}

Villain::Texture* TextureGenerator::generateTexture(int textureSize, Terrain* terrain, float minHeight, float maxHeight) {
    if (numTextureTiles == 0) {
        exitWithError("No texture tiles loaded.");
    }

    calculateTextureRegions(minHeight, maxHeight);

    int BPP = 4;
    int textureBytes = textureSize * textureSize * BPP;
    unsigned char* textureData = (unsigned char*)malloc(textureBytes);
    unsigned char* ptr = textureData;

    float heightMapToTextureRatio = terrain->getSize() / textureSize;
    printf("Height Map To Texture Ratio: %f\n", heightMapToTextureRatio);

    for (int y = 0; y < textureSize; y++) {
        for (int x = 0; x < textureSize; x++) {
            float interpolatedHeight = terrain->getHeightInterpolated(x * heightMapToTextureRatio, y * heightMapToTextureRatio);

            float red = 0.0f;
            float green = 0.0f;
            float blue = 0.0f;

            for (int tile = 0; tile < numTextureTiles; tile++) {
                glm::vec3 color = textureTiles[tile].Tex->getColor(x, y);
                float blendFactor = regionPercent(tile, interpolatedHeight);

                red += blendFactor * color.r;
                green += blendFactor * color.g;
                blue += blendFactor * color.b;
            }

            if (red > 255.f || green > 255.f || blue > 255.f) {
                printf("X: %d Y: %d RGB: %f %f %f\n", x, y, red, green, blue);
                exitWithError("Impossible terrain color.");
            }

            ptr[0] = (unsigned char)red;
            ptr[1] = (unsigned char)green;
            ptr[2] = (unsigned char)blue;
            ptr[3] = (unsigned char)255;

            ptr += BPP;
        }
    }

    Villain::Texture* texture = new Villain::Texture(GL_TEXTURE_2D);
    // Save texture for testing purposes
    //stbi_write_png("texture.png", textureSize, textureSize, BPP, textureData, textureSize * BPP);
    texture->init(textureSize, textureSize, BPP, textureData);

    free(textureData);
    return texture;
}

void TextureGenerator::calculateTextureRegions(float minHeight, float maxHeight) {
    float heightRange = maxHeight - minHeight;
    float rangePerTile = heightRange / numTextureTiles;
    float remainder = heightRange - rangePerTile * numTextureTiles;

    if (remainder < 0.0f) {
        exitWithError("Negative remainder");
    }

    float lastHeight = -1.0f;
    for (int i = 0; i < numTextureTiles; i++) {
        textureTiles[i].HeightDesc.Low = lastHeight + 1;
        lastHeight += rangePerTile;
        textureTiles[i].HeightDesc.Optimal = lastHeight;
        textureTiles[i].HeightDesc.High = textureTiles[i].HeightDesc.Optimal + rangePerTile;

        textureTiles[i].HeightDesc.print();
    }
}

float TextureGenerator::regionPercent(int tile, float height) {
    float percent = 0.0f;

    if (height < textureTiles[tile].HeightDesc.Low) {
        percent = 0.0f;
    } else if (height > textureTiles[tile].HeightDesc.High) {
        percent = 0.0f;
    } else if (height < textureTiles[tile].HeightDesc.Optimal) {
        float nom = height - textureTiles[tile].HeightDesc.Low;
        float denom = textureTiles[tile].HeightDesc.Optimal - textureTiles[tile].HeightDesc.Low;
        percent = nom / denom;
    } else if (height >= textureTiles[tile].HeightDesc.Optimal) {
        float nom = textureTiles[tile].HeightDesc.High - height;
        float denom = textureTiles[tile].HeightDesc.High - textureTiles[tile].HeightDesc.Optimal;
        percent = nom / denom;
    } else {
        exitWithError("Terrain texture calculation error.");
    }

    if (percent < 0.0f || percent > 1.0f) {
        exitWithError("Terrain texture calculation error.");
    }

    return percent;
}

