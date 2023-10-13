// (C) 2022 by ModuleWorks GmbH

#include "io/TestInput.hpp"

#include <fstream>
#include <sstream>

namespace io
{
TestInput::TestInput(const std::filesystem::path& testFile)
{
	std::ifstream f(testFile);

	if (!f)
		throw std::runtime_error("failed to open test file!");

	geo::Point3D prevPoint(0, 0, 0);

	std::string line;
	while (std::getline(f, line))
	{
		size_t i = line.find('#');
		if (i != std::string::npos)
			line.erase(i);

		std::istringstream iss(line);

		std::string cmd;
		if (!(iss >> cmd) || cmd.empty())
			continue;

		if (cmd == "cloud")
		{
			double posX, posY, posZ;
			if (!(iss >> posX >> posY >> posZ >> cloudNX >> cloudNY >> cloudNZ >> cloudDeltaS))
				throw std::runtime_error(
					"invalid \"cloud\" format! \"cloud refPoint nx ny nz delta\".");

			cloudReferencePoint.x(posX);
			cloudReferencePoint.y(posY);
			cloudReferencePoint.z(posZ);
		}
		else if (cmd == "delta")
		{
			if (!(iss >> curveDeltaT))
				throw std::runtime_error("invalid \"delta\" format! must be: \"delta deltaT\".");
		}
		else if (cmd == "sphere")
		{
			if (!(iss >> sphereRadius))
				throw std::runtime_error(
					"invalid \"sphere\" format! must be: \"sphere sphereRadius\".");
		}
		else if (cmd == "start")
		{
			double posX, posY, posZ;
			if (!(iss >> posX >> posY >> posZ))
				throw std::runtime_error("invalid \"start\" format! must be: \"start startPos\".");

			prevPoint = geo::Point3D(posX, posY, posZ);
		}
		else if (cmd == "line")
		{
			double posX, posY, posZ;
			if (!(iss >> posX >> posY >> posZ))
				throw std::runtime_error("invalid \"line\" format! must be: \"line nextPos\".");

			geo::Point3D nextPoint(posX, posY, posZ);

			curve.reset(new geo::Line(prevPoint, nextPoint));

			prevPoint = nextPoint;
		}
		else if (cmd == "arc")
		{
			double pos2_x, pos2_y, pos2_z;
			double pos3_x, pos3_y, pos3_z;
			bool isCounterClockwise;
			if (!(iss >> pos2_x >> pos2_y >> pos2_z >> pos3_x >> pos3_y >> pos3_z
				  >> isCounterClockwise))
				throw std::runtime_error(
					"invalid \"arc\" format! must be: \"arc nextPos arcCenter "
					"isCounterClockwise\"");

			geo::Point3D nextPoint(pos2_x, pos2_y, pos2_z);
			geo::Point3D arc_center(pos3_x, pos3_y, pos3_z);

			curve.reset(new geo::Arc(prevPoint, nextPoint, arc_center, isCounterClockwise));

			prevPoint = nextPoint;
		}
		else
		{
			throw std::runtime_error("unknown command!");
		}
	}
}

}  // namespace io
