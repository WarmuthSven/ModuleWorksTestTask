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
        const Point3D& refPoint,
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
        
        // Create PointCloud
        m_pointCloud = std::vector(nx,std::vector(ny,std::vector<Point3D>(nz)));
        
        double x = refPoint.x();
        for (int ix = 0; ix < m_nx; ix++)
        {
            double y = refPoint.y();
            for (int iy = 0; iy < m_ny; iy++)
            {
                double z = refPoint.z();
                for (int iz = 0; iz < m_nz; iz++)
                {
                    m_pointCloud[ix][iy][iz] = Point3D(x,y,z);
                    z+= m_deltaS;
                }
                
                y+= m_deltaS;
            }
            
            x += m_deltaS;
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
            const Curve& curve,
            const double deltaT)
    {
        if(deltaT <= 0 || deltaT > 1)
        {
            throw std::invalid_argument("Invalid argument for RemovePointsOnSpherePath. deltaT has to be greater than 0 and smaller than 1.");
        } 
        
        double startTime = 0.0;
        double endTime = deltaT;
        Point3D sphereStartPoint = curve.Evaluate(startTime);
        const double sphereRadiusSquared = std::pow(sphereRadius,2);
        
        // Iterate through time and through each point in the pointCloud
        while(endTime <= 1.0)
        {
            Point3D sphereEndPoint = curve.Evaluate(endTime);
            const Point3D cylinderAxis = sphereEndPoint - sphereStartPoint;
            
            // Gather points to delete on travel path between start and end time
            for (int ix = 0; ix < m_nx; ix++)
            {
                for (int iy = 0; iy < m_ny; iy++)
                {
                    // Iterate zVector backwards so array size can change while deleting
                    std::vector<Point3<double>>& zVector = m_pointCloud[ix][iy];
                    for (int iz = static_cast<int>(zVector.size()) - 1; iz >= 0 ; iz--)
                    {
                        const Point3D& curPoint = zVector[iz];
                        
                        // Delete points around sphere start point, end point and on travel path (cylinder)
                        if(startTime <= 0 && IsPointInSphere(curPoint, sphereStartPoint, sphereRadiusSquared)
                            || IsPointInSphere(curPoint, sphereEndPoint, sphereRadiusSquared)
                            || IsPointInCylinder(curPoint, sphereStartPoint, cylinderAxis, sphereRadiusSquared))
                        {
                            zVector.erase(zVector.begin() + iz);
                        }                       
                    }
                }
            }

            // Go to next point
            startTime = endTime;
            sphereStartPoint = sphereEndPoint;
            
            // Increase and correct next step in case it overshoots
            endTime += deltaT;
            if(startTime < 1.0 && endTime > 1.0)
            {
                endTime = 1.0;
            }
        }
    };


    /// Calculate all points visible from above and saves them to the given file path
    ///
    /// @param outputFileName name of the output file with result
    void PointCloud::CalculatePointsOnTopAndSaveToFile (
            const std::filesystem::path& outputFileName) const
    {
        const io::TestOutput to(outputFileName);

        // Write uppermost points to file
        for (auto& xyMatrix : m_pointCloud)
        {
            for (auto& zVector : xyMatrix)
            {
                if(zVector.empty())
                {
                    continue;
                }
                
                to.Write(zVector.back());
            }
        }
    }

    /// Calculate if point lies in sphere
    ///
    /// @param point point to check if in sphere
    /// @param sphereCenter center of the sphere
    /// @param sphereRadiusSquared the radius of the sphere squared
    /// @note squared radius is used to avoid use of expensive square root
    /// @return true, if point lies in sphere
    bool PointCloud::IsPointInSphere(const Point3D point, const Point3D sphereCenter, const double sphereRadiusSquared)
    {
        return (point - sphereCenter).Length2() <= sphereRadiusSquared;
    }

    /// Calculate if point lies in cylinder
    ///
    /// @param point point to check if in cylinder
    /// @param bottomPoint bottom point of the cylinder
    /// @param axis axis of the cylinder, has to contain full height of cylinder and must not be normed!
    /// @param sphereRadiusSquared the radius of the sphere squared
    /// @note squared radius is used to avoid use of expensive square root
    /// @return true, if point lies in cylinder
    bool PointCloud::IsPointInCylinder(const Point3D point, const Point3D bottomPoint, const Point3D axis, const double sphereRadiusSquared)
    {
        // Calculate perpendicular distance of point to axis
        // If lineSegment lies not on the axis, disregard it
        // Algorithm Basics:
        // (1) (curPoint - orthoIntersectionPoint) * axis = 0
        // (2) orthoIntersectPoint = bottomPoint + axis * lineSegment
        const double lineSegment = (point * axis - bottomPoint * axis) / (axis * axis);
        
        if(lineSegment < 0 || lineSegment > 1)
        {
            return false;
        }
        
        const Point3D orthoIntersectPoint = bottomPoint + axis * lineSegment;
        
        return (point - orthoIntersectPoint).Length2() <= sphereRadiusSquared;
    }
}
