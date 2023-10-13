﻿#include "calc/CalcVisPoints.hpp"
#include "io/TestOutput.hpp"

#include <string>
#include <vector>

namespace calc
{
    /// Calculate the result and output it to the outputFileName
    ///
    ///	@param refPoint reference point O of the cloud, which is a point with the minimum values along
    /// all coordinate axes
    ///	@param nx number of points in cloud along x axis
    ///	@param ny number of points in cloud along y axis
    ///	@param nz number of points in cloud along z axis
    ///	@param deltaS distance between neighboring cloud points along x, y and z axis
    ///	@param sphereRadius radius R of the sphere
    ///	@param curve 3d curve that defines trajectory of the sphere
    ///	@param deltaT step size for 3d curve parameter
    ///	@param outputFileName name of the output file with result
    void Calculate(
        const geo::Point3D& refPoint,
        const int nx,
        const int ny,
        const int nz,
        const double deltaS,
        const double sphereRadius,
        const geo::Curve& curve,
        const double deltaT,
        const std::filesystem::path& outputFileName)
    {
        io::TestOutput to(outputFileName);
        // Implement your solution here:

        // TODO: output resulting points
        to.Write(geo::Point3D(0, 0, 0));
    }
}
