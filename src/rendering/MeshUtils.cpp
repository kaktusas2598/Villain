#include "MeshUtils.hpp"

namespace Villain {

    void MeshUtils::addFace(std::vector<unsigned int>* indices, int startLocation, bool direction) {
        if (direction) {
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 1);
            indices->push_back(startLocation + 0);
            indices->push_back(startLocation + 3);
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 0);
        } else {
            indices->push_back(startLocation + 0);
            indices->push_back(startLocation + 1);
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 0);
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 3);
        }
    }
}

