#pragma once

#include "geo/Curve.hpp"
#include "geo/Point3.hpp"

#include <filesystem>

namespace geo
{
    class PointCloud
    {
    public: PointCloud(
        const geo::Point3D& refPoint,
        const int nx,
        const int ny,
        const int nz,
        const double deltaS);

        void RemovePointsOnSpherePath(
            const double sphereRadius,
            const geo::Curve& curve,
            const double deltaT);

        void CalculatePointsOnTopAndSaveToFile(
            const std::filesystem::path& outputFileName);
    private:
        const geo::Point3D m_refPoint;
        const int m_nx;
        const int m_ny;
        const int m_nz;
        const double m_deltaS;
        std::vector<std::vector<std::vector<geo::Point3D>>> m_pointCloud;
    };
}
