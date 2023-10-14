// (C) 2022 by ModuleWorks GmbH

#include "io/TestInput.hpp"
#include "calc/CalcVisPoints.hpp"

#include <filesystem>
#include <iostream>

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

		calc::Calculate(
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
