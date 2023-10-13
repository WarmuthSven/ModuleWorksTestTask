// (C) 2022 by ModuleWorks GmbH
#pragma once

#include "geo/Curve.hpp"
#include "geo/Point3.hpp"

#include <filesystem>
#include <vector>


namespace io
{
struct TestInput
{
	TestInput(const std::filesystem::path& testFile);

	geo::Point3D cloudReferencePoint = geo::Point3D(0., 0., 0.);
	int cloudNX = 100;
	int cloudNY = 100;
	int cloudNZ = 100;
	double cloudDeltaS = 0.1;

	double curveDeltaT = 0.1;
	double sphereRadius = 1.;

	std::unique_ptr<geo::Curve> curve;
};

}  // namespace io
