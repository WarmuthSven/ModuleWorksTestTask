// (C) 2022 by ModuleWorks GmbH

#include "io/TestOutput.hpp"

#include <stdexcept>

namespace io
{
TestOutput::TestOutput(const std::filesystem::path& outputFile)
{
	m_fd = NULL;
#ifdef _WIN32
	_wfopen_s(&m_fd, outputFile.c_str(), L"wt");
#else
	m_fd = fopen(outputFile.c_str(), "wt");
#endif
	if (m_fd == NULL)
	{
		throw std::runtime_error("Cannot open output file");
	}
}
TestOutput::~TestOutput()
{
	if (m_fd)
	{
		fclose(m_fd);
	}
}
void TestOutput::Write(const geo::Point3D& p) const
{
	fprintf(m_fd, "%lf %lf %lf\n", p.x(), p.y(), p.z());
}
}  // namespace io
