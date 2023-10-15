#include "calc/CalcVisPoints.hpp"
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
        // TODO: Check if nx,ny,nz,deltaS,sphereRadius,deltaT are positive
        
        io::TestOutput to(outputFileName);
        
        // Implement your solution here:
        std::vector pointCloud(nx,std::vector(ny,std::vector<geo::Point3D>(nz)));
        
        // Create PointCloud
        double x = refPoint.x();
        for (int ix = 0; ix < nx; ix++)
        {
            double y = refPoint.y();
            for (int iy = 0; iy < ny; iy++)
            {
                double z = refPoint.z();
                for (int iz = 0; iz < nz; iz++)
                {
                    pointCloud[ix][iy][iz] = geo::Point3D(x,y,z);
                    z+= deltaS;
                }
                
                y+= deltaS;
            }
            
            x += deltaS;
        }

        // Remove points along trajectory of sphere
        double t1 = 0.0;
        double t2 = deltaT;
        const double sphereRadiusSquared = std::pow(sphereRadius,2.0);
        std::vector<geo::Point3I> deletePoints;
        while(t2 <= 1.0)
        {
            geo::Point3D sphereStartPoint = curve.Evaluate(t1);
            geo::Point3D sphereEndPoint = curve.Evaluate(t2);
            geo::Point3D direction = sphereEndPoint - sphereStartPoint;
            
            // Gather points to delete on travel path between timesteps t1 and t2
            for (int ix = 0; ix < pointCloud.size(); ix++)
            {
                for (int iy = 0; iy < pointCloud[ix].size(); iy++)
                {
                    for (int iz = 0; iz < pointCloud[ix][iy].size(); iz++)
                    {
                        geo::Point3D curPoint = pointCloud[ix][iy][iz];
                        
                        // Add deleted points around sphere start point
                        geo::Point3D distance = curPoint - sphereStartPoint;
                        if(distance.Length2() <= sphereRadiusSquared)
                        {
                            deletePoints.emplace_back(geo::Point3I(ix,iy,iz));
                            continue;
                        }

                        // Only check travel endpoint at last time step
                        if(t2 >= 1.0)
                        {
                            //Add deleted points around sphere end point
                            distance = curPoint - sphereEndPoint;
                            if(distance.Length2() <= sphereRadiusSquared)
                            {
                                deletePoints.emplace_back(geo::Point3I(ix,iy,iz));
                                continue;
                            }
                        }
                        
                        // Calculate perpendicular distance of point to direction of travel
                        // If lineSegment is not between start and end point, disregard it
                        // Algorithm Basics:
                        // (1) (curPoint - orthoIntersectionPoint) * direction = 0
                        // (2) orthoIntersectionPoint = sphereStartPoint + lineSegment * direction
                        // Note: direction in equation (2) must not be normed
                        const double lineSegment = (curPoint * direction - sphereStartPoint * direction) / (direction * direction);
                        if(lineSegment < 0 || lineSegment > 1)
                        {
                            continue;
                        }
                        geo::Point3D orthoIntersectPoint = sphereStartPoint + direction * lineSegment;

                        // Add deleted points on linear path of sphere
                        distance = curPoint - orthoIntersectPoint;
                        if(distance.Length2() <= sphereRadiusSquared)
                        {
                            deletePoints.emplace_back(geo::Point3I(ix,iy,iz));
                            continue;
                        }
                    }
                }
            }
            
            // Delete points backwards so cached array positions stay correct when Array size changes
            for(int i = deletePoints.size() - 1; i >= 0; i--)
            {
                geo::Point3I point = deletePoints[i];
                pointCloud[point.x()][point.y()].erase(pointCloud[point.x()][point.y()].begin() + point.z());
            }
            deletePoints.clear();

            // Increase Timestep
            t1 = t2;
            t2 += deltaT;
            
            // Correct last step in case it overshoots
            if(t1 < 1.0 && t2 > 1.0)
            {
                t2 = 1.0;
            }
        }
        
        // Write uppermost points to file
        for (int ix = 0; ix < pointCloud.size(); ix++)
        {
            for (int iy = 0; iy < pointCloud[ix].size(); iy++)
            {
                to.Write(pointCloud[ix][iy].back());
            }
        }
    }
}
