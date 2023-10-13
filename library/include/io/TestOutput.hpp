// (C) 2022 by ModuleWorks GmbH
#pragma once

#include "geo/Point3.hpp"

#include <cstdio>
#include <filesystem>
#include <string>


namespace io
{
class TestOutput
{
public:
	TestOutput(const std::filesystem::path& f);

	~TestOutput();

	void Write(const geo::Point3D& p) const;

private:
	FILE* m_fd;
};
}  // namespace io
