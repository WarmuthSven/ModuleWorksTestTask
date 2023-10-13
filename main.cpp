// (C) 2022 by ModuleWorks GmbH

#include "geo/Curve.hpp"
#include "geo/Point3.hpp"
#include "io/TestInput.hpp"
#include "io/TestOutput.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

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


int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "Usage: cutSphereMove <test_in_file> <results_file>" << std::endl;
		return 1;
	}

	std::filesystem::path testInput = argv[1];
	std::filesystem::path testOutput = argv[2];

	try
	{
		io::TestInput test(testInput);

		Calculate(
			test.cloudReferencePoint,
			test.cloudNX,
			test.cloudNY,
			test.cloudNZ,
			test.cloudDeltaS,
			test.sphereRadius,
			*test.curve,
			test.curveDeltaT,
			testOutput);
	}
	catch (std::exception& e)
	{
		std::cout << "std::exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
