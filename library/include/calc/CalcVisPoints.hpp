#pragma once

#include "geo/Curve.hpp"
#include "geo/Point3.hpp"

#include <filesystem>

namespace calc
{
    void Calculate(
        const geo::Point3D& refPoint,
        const int nx,
        const int ny,
        const int nz,
        const double deltaS,
        const double sphereRadius,
        const geo::Curve& curve,
        const double deltaT,
        const std::filesystem::path& outputFileName);
}
