#include "geo/PointCloud.hpp"

#include "io/TestOutput.hpp"

#include <vector>

namespace geo
{
    /// Create PointCloud
    ///
    ///	@param refPoint reference point O of the cloud, which is a point with the minimum values along
    /// all coordinate axes
    ///	@param nx number of points in cloud along x axis
    ///	@param ny number of points in cloud along y axis
    ///	@param nz number of points in cloud along z axis
    ///	@param deltaS distance between neighboring cloud points along x, y and z axis
    ///	@throws std::invalid_argument if any of the parameters (nx, ny, nz or deltaS) are not greater than 0.
    PointCloud::PointCloud(
        const geo::Point3D& refPoint,
        const int nx,
        const int ny,
        const int nz,
        const double deltaS): m_refPoint(refPoint),
        m_nx(nx),
        m_ny(ny),
        m_nz(nz),
        m_deltaS(deltaS)
    {
        if(nx <= 0 || ny <= 0 || nz <= 0 || deltaS <= 0)
        {
            throw std::invalid_argument("Invalid argument for PointCloud Constructor. nx, ny, nz and deltaS have to be greater than 0.");
        }
        
        m_pointCloud = std::vector(nx,std::vector(ny,std::vector<geo::Point3D>(nz)));
        
        // Create PointCloud
        double x = refPoint.x();
        for (int ix = 0; ix < m_nx; ix++)
        {
            double y = refPoint.y();
            for (int iy = 0; iy < m_ny; iy++)
            {
                double z = refPoint.z();
                for (int iz = 0; iz < m_nz; iz++)
                {
                    m_pointCloud[ix][iy][iz] = geo::Point3D(x,y,z);
                    z+= deltaS;
                }
                
                y+= deltaS;
            }
            
            x += deltaS;
        }
    }

    /// Removes all points on the trajectory of the sphere
    /// @note Curve is linearly interpolated between steps
    /// 
    ///	@param sphereRadius radius R of the sphere
    ///	@param curve 3d curve that defines trajectory of the sphere
    ///	@param deltaT step size for 3d curve parameter
    ///	@throws std::invalid_argument if deltaT is not greater than 0.
    void PointCloud::RemovePointsOnSpherePath(
            const double sphereRadius,
            const geo::Curve& curve,
            const double deltaT)
    {
        if(deltaT <= 0)
        {
            throw std::invalid_argument("Invalid argument for RemovePointsOnSpherePath. deltaT has to be greater than 0.");
        }
        
        // Remove points along trajectory of sphere
        double startTime = 0.0;
        double endTime = deltaT;
        const double sphereRadiusSquared = std::pow(sphereRadius,2.0);
        std::vector<geo::Point3I> deletePoints;
        while(endTime <= 1.0)
        {
            geo::Point3D sphereStartPoint = curve.Evaluate(startTime);
            geo::Point3D sphereEndPoint = curve.Evaluate(endTime);
            geo::Point3D direction = sphereEndPoint - sphereStartPoint;
            
            // Gather points to delete on travel path between start and end time
            for (int ix = 0; ix < m_pointCloud.size(); ix++)
            {
                for (int iy = 0; iy < m_pointCloud[ix].size(); iy++)
                {
                    for (int iz = 0; iz < m_pointCloud[ix][iy].size(); iz++)
                    {
                        geo::Point3D curPoint = m_pointCloud[ix][iy][iz];
                        
                        // Add deleted points around sphere start point
                        geo::Point3D distance = curPoint - sphereStartPoint;
                        if(distance.Length2() <= sphereRadiusSquared)
                        {
                            deletePoints.emplace_back(geo::Point3I(ix,iy,iz));
                            continue;
                        }

                        // Only check travel endpoint at last time step
                        if(endTime >= 1.0)
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
                m_pointCloud[point.x()][point.y()].erase(m_pointCloud[point.x()][point.y()].begin() + point.z());
            }
            deletePoints.clear();

            // Increase Timestep
            startTime = endTime;
            endTime += deltaT;
            
            // Correct last step in case it overshoots
            if(startTime < 1.0 && endTime > 1.0)
            {
                endTime = 1.0;
            }
        }
    };


    /// Calculate all points visible from above and saves them to the given file path
    ///
    /// @param outputFileName name of the output file with result
    void PointCloud::CalculatePointsOnTopAndSaveToFile(
            const std::filesystem::path& outputFileName)
    {
        io::TestOutput to(outputFileName);

        // Write uppermost points to file
        for (int ix = 0; ix < m_pointCloud.size(); ix++)
        {
            for (int iy = 0; iy < m_pointCloud[ix].size(); iy++)
            {
                if(m_pointCloud[ix][iy].empty())
                {
                    continue;
                }
                
                to.Write(m_pointCloud[ix][iy].back());
            }
        }
    }
}
