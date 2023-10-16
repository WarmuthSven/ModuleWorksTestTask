#pragma once

#include "geo/Curve.hpp"
#include "geo/Point3.hpp"

#include <filesystem>

namespace geo
{
    class PointCloud
    {
    public: PointCloud(
        const Point3D& refPoint,
        const int nx,
        const int ny,
        const int nz,
        const double deltaS);

        void RemovePointsOnSpherePath(const double sphereRadius, const Curve& curve, const double deltaT);
        void CalculatePointsOnTopAndSaveToFile(const std::filesystem::path& outputFileName) const;
    private:
        static bool IsPointInSphere(const Point3D point, const Point3D sphereCenter, const double sphereRadiusSquared);
        static bool IsPointInCylinder(const Point3D point, const Point3D bottomPoint, const Point3D axis, const double sphereRadiusSquared);
        
        const Point3D m_refPoint;
        const int m_nx;
        const int m_ny;
        const int m_nz;
        const double m_deltaS;
        std::vector<std::vector<std::vector<Point3D>>> m_pointCloud;
    };
}
