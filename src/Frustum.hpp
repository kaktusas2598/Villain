#pragma once

#include "physics/Plane.hpp"

namespace Villain {

    struct Frustum {
        Plane topFace;
        Plane bottomFace;

        Plane leftFace;
        Plane rightFace;

        Plane nearFace;
        Plane farFace;
    };
}
