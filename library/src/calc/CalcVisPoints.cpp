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

        // Create Base PointCloud
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
            // Delete points in Sphere at Start and Endpoint
            for (int ix = 0; ix < pointCloud.size(); ix++)
            {
                for (int iy = 0; iy < pointCloud[ix].size(); iy++)
                {
                    for (int iz = 0; iz < pointCloud[iy].size(); iz++)
                    {
                        geo::Point3D curPoint = pointCloud[ix][iy][iz];

                        //Add deleted elements around sphere start point
                        geo::Point3D distance = curPoint - sphereStartPoint;
                        if(distance.Length2() <= sphereRadiusSquared)
                        {
                            deletePoints.emplace_back(geo::Point3I(ix,iy,iz));
                            break;
                        }

                        //Only check end of travel at last time frame
                        if(t2 >= 1.0)
                        {
                            //Add deleted elements around sphere end point
                            distance = curPoint - sphereEndPoint;
                            if(distance.Length2() <= sphereRadiusSquared)
                            {
                                deletePoints.emplace_back(geo::Point3I(ix,iy,iz));
                                break;
                            }
                        }
                        

                        //Add deleted elements on linear path
                        const double pathwaySegment = (curPoint * direction - sphereStartPoint * direction) / (direction * direction);
                        if(pathwaySegment < 0 || pathwaySegment > 1)
                        {
                            break;
                        }

                        geo::Point3D orthoIntersectPoint = sphereStartPoint + direction * pathwaySegment;
                        distance = curPoint - orthoIntersectPoint;
                        if(distance.Length2() <= sphereRadiusSquared)
                        {
                            deletePoints.emplace_back(geo::Point3I(ix,iy,iz));
                            break;
                        }
                    }
                }
            }
            
            // Delete points backwards so positions stay correct when Vector size changes
            for(int i = deletePoints.size() - 1; i >= 0; i--)
            {
                geo::Point3I point = deletePoints[i];
                pointCloud[point.x()][point.y()].erase(pointCloud[point.x()][point.y()].begin() + point.z());
            }
            
            t1 = t2;
            t2 += deltaT;
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
