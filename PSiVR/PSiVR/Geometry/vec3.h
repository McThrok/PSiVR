#pragma once
#include <math.h>

class vec3
{
public:
	long double x, y, z;

	vec3() : vec3(0, 0, 0) {}
	vec3(long double _x, long double _y, long double _z) : x(_x), y(_y), z(_z) {}
	vec3(const vec3& u)
	{
		x = u.x;
		y = u.y;
		z = u.z;
	}

	vec3& operator=(const vec3& u)
	{
		if (this != &u)
		{
			x = u.x;
			y = u.y;
			z = u.z;
		}
		return *this;
	}
	vec3& operator+=(const vec3& u)
	{
		x += u.x;
		y += u.y;
		z += u.z;
		return *this;
	}
	vec3& operator-=(const vec3& u)
	{
		x -= u.x;
		y -= u.y;
		z -= u.z;
		return *this;
	}
	vec3& operator*=(const vec3& u)
	{
		x *= u.x;
		y *= u.y;
		z *= u.z;
		return *this;
	}
	vec3& operator/=(const vec3& u)
	{
		x /= u.x;
		y /= u.y;
		z /= u.z;
		return *this;
	}
	vec3& operator+=(long double t)
	{
		x += t;
		y += t;
		z += t;
		return *this;
	}
	vec3& operator-=(long double t)
	{
		x -= t;
		y -= t;
		z -= t;
		return *this;
	}
	vec3& operator*=(long double t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}
	vec3& operator/=(long double t)
	{
		return *this *= (1 / t);
	}
	const vec3 operator+(const vec3& u) const
	{
		return vec3(*this) += u;
	}
	const vec3 operator-(const vec3& u) const
	{
		return vec3(*this) -= u;
	}
	const vec3 operator*(const vec3& u) const
	{
		return vec3(*this) *= u;
	}
	const vec3 operator/(const vec3& u) const
	{
		return vec3(*this) /= u;
	}
	const vec3 operator+(long double t) const
	{
		return vec3(*this) += t;
	}
	const vec3 operator-(long double t) const
	{
		return vec3(*this) -= t;
	}
	const vec3 operator*(long double t) const
	{
		return vec3(*this) *= t;
	}
	const vec3 operator/(long double t) const
	{
		return vec3(*this) *= (1 / t);
	}

	vec3 operator-()
	{
		return vec3(*this) *= -1.0f;
	}
	bool operator==(const vec3& u) const
	{
		return (x == u.x) && (y == u.y) && (z == u.z);
	}
	bool operator!=(const vec3& u) const
	{
		return (x != u.x) || (y != u.y) || (z != u.z);
	}

	long double dot(const vec3& u) const
	{
		return x * u.x + y * u.y + z * u.z;
	}
	vec3 cross(const vec3& u) const
	{
		return vec3(y * u.z - z * u.y, z * u.x - x * u.z, x * u.y - y * u.x);
	}
	vec3 reflect(const vec3& u) const
	{
		return u * (*this).dot(u) / u.length() / u.length() * 2 - *this;
	}
	long double length() const
	{
		return sqrtl(x * x + y * y + z * z);
	}
	void normalize()
	{
		long double len = length();
		if (len > 0)
			*this /= len;
	}
	vec3 normalized() const
	{
		vec3 v(*this);
		v.normalize();
		return v;
	}
};

const vec3 operator+(const long double& t, const vec3& v);
const vec3 operator-(const long double& t, const vec3& v);
const vec3 operator*(const long double& t, const vec3& v);
const vec3 operator/(const long double& t, const vec3& v);
