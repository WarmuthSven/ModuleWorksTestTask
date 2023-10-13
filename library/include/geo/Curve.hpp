// (C) 2022 by ModuleWorks GmbH

#pragma once

#include "geo/Point3.hpp"

#include <stdexcept>

namespace geo
{
/// Base class for motion functions
class Curve
{
public:
	Curve(){};

	virtual ~Curve(){};

	/// Evaluate at a given t
	virtual geo::Point3D Evaluate(const double t) const = 0;

	/// Get the begin of the parameter interval
	double GetBeginParameter() const { return 0; };

	//! Get the end of the parameter interval
	double GetEndParameter() const { return 1; };

protected:
	void CheckT(const double t) const
	{
		if (t < 0. || t > 1.)
			throw std::runtime_error("T is out of range");
	}
};

/// This class describes function of linear motion for t in [0, 1] interval
class Line : public Curve
{
public:
	Line(const geo::Point3D& ptStart, const geo::Point3D& ptEnd)
		: m_ptStart(ptStart), m_ptEnd(ptEnd)
	{
	}

	geo::Point3D Evaluate(const double t) const override
	{
		CheckT(t);
		return m_ptStart * (1.0 - t) + m_ptEnd * (t);
	}

private:
	const geo::Point3D m_ptStart;
	const geo::Point3D m_ptEnd;
};


/// This class implements function of arc motion
/// from arcStart to arcEnd in counter-clockwise direction
/// for t in [0, 1] interval
class Arc : public Curve
{
public:
	static constexpr double pi = 3.1415926535897932384626433832795;
	static constexpr double tolerance = 1e-12;

	Arc(const geo::Point3D& arcStart,
		const geo::Point3D& arcEnd,
		const geo::Point3D& arcCenter,
		const bool isCounterClockwise)
	{
		geo::Point3D centerToStartVector = arcStart - arcCenter;
		geo::Point3D centerToEndVector = arcEnd - arcCenter;
		const double radiusToStart =
			~geo::Point3D(centerToStartVector.x(), centerToStartVector.y(), 0);
		const double radiusToEnd = ~geo::Point3D(centerToEndVector.x(), centerToEndVector.y(), 0);
		m_origin = arcCenter;
		m_origin.z(arcStart.z());
		m_radius = radiusToStart;
		m_zHeight = arcEnd.z() - arcStart.z();
		double angleToStart = std::atan2(centerToStartVector.y(), centerToStartVector.x());
		double angleToEnd = std::atan2(centerToEndVector.y(), centerToEndVector.x());
		if (angleToStart < 0)
		{
			angleToStart += 2 * pi;
		}
		if (angleToEnd < 0)
		{
			angleToEnd += 2 * pi;
		}
		m_startAngle = angleToStart;
		if (!isCounterClockwise)
		{
			std::swap(angleToStart, angleToEnd);
		}
		m_sweepAngle = angleToEnd - angleToStart;
		if (angleToStart > angleToEnd)
		{
			m_sweepAngle = 2 * pi + m_sweepAngle;
		}
		if (std::abs(m_sweepAngle) < tolerance)
		{
			m_sweepAngle = 2 * pi;
		}
		if (!isCounterClockwise)
		{
			m_sweepAngle *= -1;
		}
	}

	geo::Point3D Evaluate(const double t) const override
	{
		CheckT(t);
		const double x = m_radius * std::cos(m_startAngle + m_sweepAngle * t);
		const double y = m_radius * std::sin(m_startAngle + m_sweepAngle * t);
		const double z = m_zHeight * t;
		return m_origin + geo::Point3D(x, y, z);
	}

private:
	geo::Point3D m_origin;
	double m_radius;
	double m_startAngle;
	double m_sweepAngle;
	double m_zHeight;
};
}  // namespace geo