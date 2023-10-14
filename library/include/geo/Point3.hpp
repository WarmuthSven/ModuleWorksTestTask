// (C) 2022 by ModuleWorks GmbH

#pragma once

#include <cmath>
#include <stdexcept>

namespace geo
{
template <class T>
struct Point3
{
public:
	typedef T ValueType;

	/// Sets 3d point to origin.
	Point3()
	{
		m_elems[0] = 0;
		m_elems[1] = 0;
		m_elems[2] = 0;
	}

	/// Creates 3d point
	///
	/// Creates the 3d point to the new position.
	///	@param first constant x-coordinate of the 3d point
	///	@param second constant y-coordinate of the 3d point
	///	@param third constant z-coordinate of the 3d point
	Point3(const T first, const T second, const T third)
	{
		m_elems[0] = first;
		m_elems[1] = second;
		m_elems[2] = third;
	}

	/// Are given points equal?
	/// @param tc tool to compare the current tool with
	/// @returns <b>true</b> if both tools contain the same parameters,
	/// <b>false</b> otherwise
	bool operator==(const Point3& tc) const
	{
		return (
			m_elems[0] == tc.m_elems[0] && m_elems[1] == tc.m_elems[1]
			&& m_elems[2] == tc.m_elems[2]);
	}

	/// Are given points equal?
	/// @param tc tool to compare the current tool with
	/// @returns <b>false</b> if both tools contain the same parameters,
	/// <b>true</b> otherwise
	bool operator!=(const Point3& tc) const { return !((*this) == tc); }

	/// Get z-coordinate
	///
	/// Gets the z-coordinate of the 3d point as a reference.
	///	@returns constant reference to the z-coordinate
	T z() const { return m_elems[2]; }

	/// Get y-coordinate
	///
	/// Gets the y-coordinate of the 3d point as a reference.
	///	@returns constant reference to the y-coordinate
	T y() const { return m_elems[1]; }

	/// Get x-coordinate
	///
	/// Gets the x-coordinate of the 3d point as a reference.
	///	@returns constant reference to the x-coordinate
	T x() const { return m_elems[0]; }

	/// Set 3d point
	///
	/// Sets new z-coordinate of the 3d point.
	///	@param newz const z-coordinate of the 3d point
	void z(const T newz) { m_elems[2] = newz; }

	/// Set 3d point
	///
	/// Sets new y-coordinate of the 3d point.
	///	@param newy const y-coordinate of the 3d point
	void y(const T newy) { m_elems[1] = newy; }

	/// Set 3d point
	///
	/// Sets new x-coordinate of the 3d point.
	///	@param newx const x-coordinate of the 3d point
	void x(const T newx) { m_elems[0] = newx; }

	/// Subscript operator, get coordinate value by index
	T& operator[](const size_t index) { return m_elems[index]; }

	/// Subscript operator, get coordinate value by index
	const T& operator[](const size_t index) const { return m_elems[index]; }

	/// Subtract each component of a 3d point with another
	///	@param ta const reference of a 3d point
	Point3<T>& operator-=(const Point3& ta)
	{
		m_elems[0] -= ta.m_elems[0];
		m_elems[1] -= ta.m_elems[1];
		m_elems[2] -= ta.m_elems[2];
		return *this;
	}

	/// Add each component of a 3d point to another
	///	@param ta const reference of a 3d point
	Point3<T>& operator+=(const Point3& ta)
	{
		m_elems[0] += ta.m_elems[0];
		m_elems[1] += ta.m_elems[1];
		m_elems[2] += ta.m_elems[2];
		return *this;
	}

	/// Returns length of the vector represented by the 3d point
	///	@returns constant length of the vector represented by the 3d point.
	const T operator~() const
	{
		return static_cast<T>(::sqrt(
			// for the generic implementation don't cast to double
			// since this might be used with types that can't be
			// recovered from double (eg mwAutodiff::ScalarT)
			m_elems[0] * m_elems[0] + m_elems[1] * m_elems[1] + m_elems[2] * m_elems[2]));
	}

	/// Multiply each 3d component with a factor
	///	@param scaleFactor const reference to a factor
	Point3<T>& operator*=(const T scaleFactor)
	{
		m_elems[0] *= scaleFactor;
		m_elems[1] *= scaleFactor;
		m_elems[2] *= scaleFactor;
		return *this;
	}

	/// Divide each 3d component by a factor
	///	@param scaleFactor const reference to a factor
	Point3<T>& operator/=(const T scaleFactor)
	{
		m_elems[0] /= scaleFactor;
		m_elems[1] /= scaleFactor;
		m_elems[2] /= scaleFactor;
		return *this;
	}

	/// Squared length
	///
	/// Returns squared length of the vector represented by the 3d point
	///	@returns constant length of the vector represented by the 3d point.
	T Length2() const
	{
		return (*this) * (*this);  // reuse scalar multiplication
	}

	/// IsTolerant function
	///
	///	@param src Point to compare to
	///	@param factor distance
	///	@returns true if points are within factor distance to each other
	bool IsTolerant(const Point3& src, const T factor /*= mathdef::mw_tol(T())*/) const
	{
		return (src - *this).Length2() < factor * factor;
	}

	/// True if it's a null-vector
	bool IsExactlyZero() const { return (m_elems[0] == 0 && m_elems[1] == 0 && m_elems[2] == 0); }

	/// Normalize this vector
	const Point3<T>& Normalize()
	{
		const auto length = ~(*this);
		if (length <= 0)
			throw std::runtime_error("Cannot normalize 0 vector");
		(*this) /= length;
		return *this;
	}

	/// Get normalized version of this vector
	Point3<T> Normalized() const
	{
		Point3 result(*this);
		result.Normalize();
		return result;
	}

private:
	T m_elems[3];
};

template <class T>
inline Point3<T> operator-(const Point3<T>& p);

template <class T>
inline Point3<T> operator+(const Point3<T>& first, const Point3<T>& second);

template <class T>
inline Point3<T> operator-(const Point3<T>& first, const Point3<T>& second);

template <class T>
inline Point3<T> operator*(const Point3<T>& pt, const T factor);

template <class T>
inline Point3<T> operator*(const T factor, const Point3<T>& pt);

template <class T>
inline Point3<T> operator/(const Point3<T>& pt, const T factor);

template <class T>
inline T operator*(const Point3<T>& first, const Point3<T>& second);

template <class T>
inline Point3<T> operator%(const Point3<T>& first, const Point3<T>& second);


/// Change sign of each 3d point component to opposite
/// @param p const reference of a 3d point
/// @returns negated const 3d point
template <class T>
inline Point3<T> operator-(const Point3<T>& p)
{
	return Point3<T>(-p.x(), -p.y(), -p.z());
}

/// Add each component of a 3d point to another
/// @param first const reference of a 3d point
/// @param second const reference of a 3d point
/// @returns added const 3d point
template <class T>
inline Point3<T> operator+(const Point3<T>& first, const Point3<T>& second)
{
	return Point3<T>(first.x() + second.x(), first.y() + second.y(), first.z() + second.z());
}

/// Subtract each 3d point component from another
/// @param first const reference of a 3d point
/// @param second const reference of a 3d point
/// @returns subtracted const 3d point
template <class T>
inline Point3<T> operator-(const Point3<T>& first, const Point3<T>& second)
{
	return Point3<T>(first.x() - second.x(), first.y() - second.y(), first.z() - second.z());
}

/// Multiply each 3d component with a factor.
/// @param pt const reference to a 3d point
/// @param factor const reference to a factor to multiply with
/// @returns new 3d point object
template <class T>
inline Point3<T> operator*(const Point3<T>& pt, const T factor)
{
	return Point3<T>(pt.x() * factor, pt.y() * factor, pt.z() * factor);
}

/// Multiply each 3d component with a factor.
/// @param factor const reference to a factor to multiply with
/// @param pt const reference to a 3d point
/// @returns new 3d point object
template <class T>
inline Point3<T> operator*(const T factor, const Point3<T>& pt)
{
	return Point3<T>(pt.x() * factor, pt.y() * factor, pt.z() * factor);
}

/// Divide each 3d component with a factor.
/// @param pt const reference to a 3d point
/// @param factor const reference to a factor to divide with
/// @returns new 3d point object
template <class T>
inline Point3<T> operator/(const Point3<T>& pt, const T factor)
{
	return Point3<T>(pt.x() / factor, pt.y() / factor, pt.z() / factor);
}

/// Scalar product of to 3d points.
/// @param first const reference to a 3d point
/// @param second const reference to a 3d point
/// @returns const scalar
template <class T>
inline T operator*(const Point3<T>& first, const Point3<T>& second)
{
	return first.x() * second.x() + first.y() * second.y() + first.z() * second.z();
}

/// Cross (vector) product of to 3d points
/// @param first const reference to a 3d point
/// @param second const reference to a 3d point
/// @returns const 3d point
template <class T>
inline Point3<T> operator%(const Point3<T>& first, const Point3<T>& second)
{
	return Point3<T>(
		first.y() * second.z() - first.z() * second.y(),
		first.z() * second.x() - first.x() * second.z(),
		first.x() * second.y() - first.y() * second.x());
}

using Point3D = Point3<double>;
using Point3F = Point3<float>;
using Point3I = Point3<int>;
}  // namespace geo