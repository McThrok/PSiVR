#pragma once

#include <math.h>
#include "vec3.h"
#include "mat3.h"

class quat
{
public:
	float x, y, z, w;

	quat() :x(0), y(0), z(0), w(0) {}
	quat(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w) {}
	quat(vec3 v, float _w) :x(v.x), y(v.y), z(v.z), w(_w) {}

	quat identity() const
	{
		return quat(0, 0, 0, 1);
	}
	quat inv() const
	{
		return quat(-x, -y, -z, w);

	}
	mat3 rotationMat()
	{
		return rotationMat(*this);
	}
	mat3 rotationMat(const quat& quaternion)
	{
		quat q = quaternion.normalized();
		mat3 mat;
		mat.m[0][0] = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
		mat.m[0][1] = 2 * q.x * q.y - 2 * q.z * q.w;
		mat.m[0][2] = 2 * q.x * q.z + 2 * q.y * q.w;
		mat.m[1][0] = 2 * q.x * q.y + 2 * q.z * q.w;
		mat.m[1][1] = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
		mat.m[1][2] = 2 * q.y * q.z - 2 * q.x * q.w;
		mat.m[2][0] = 2 * q.x * q.z - 2 * q.y * q.w;
		mat.m[2][1] = 2 * q.y * q.z + 2 * q.x * q.w;
		mat.m[2][2] = 1 - 2 * q.x * q.x - 2 * q.y * q.y;

		return mat;
	}

	vec3 rotateVec(const vec3& v) const
	{
		quat ads = (*this) * quat(v, 0);
		quat result = ((*this) * quat(v, 0)) * inv();
		return vec3(result.x, result.y, result.z);
	}

	quat(const quat& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
	}
	quat& operator=(const quat& q)
	{
		if (this != &q)
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;
		}
		return *this;
	}

	quat& operator+=(const quat& q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
		return *this;
	}
	quat& operator-=(const quat& q)
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;
		return *this;
	}
	quat& operator*=(const quat& q)
	{
		//float newX = w * q.x + x * q.w + y * q.z - z * q.y;
		//float newY = w * q.y - x * q.z + y * q.w + z * q.x;
		//float newZ = w * q.z + x * q.y - y * q.x + z * q.w;
		//float newW = w * q.w - x * q.x - y * q.y - z * q.z;

		//newX = x * q.w + y * q.z - z * q.y + w * q.x;
		//newY = -x * q.z + y * q.w + z * q.x + w * q.y;
		//newZ = x * q.y - y * q.x + z * q.w + w * q.z;
		//newW = -x * q.x - y * q.y - z * q.z + w * q.w;


		float newX = (w * q.x) + (x * q.w) + (y * q.z) - (z * q.y);
		float newY = (w * q.y) - (x * q.z) + (y * q.w) + (z * q.x);
		float newZ = (w * q.z) + (x * q.y) - (y * q.x) + (z * q.w);
		float newW = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);

		//float newW = w * q.w - x * q.x - y * q.y - z * q.z;
		//float newX = w * q.x + x * q.w - y * q.z + z * q.y;
		//float newY = w * q.y + x * q.z + y * q.w - z * q.x;
		//float newZ = w * q.z - x * q.y + y * q.x + z * q.w;

		x = newX;
		y = newY;
		z = newZ;
		w = newW;

		return *this;
	}
	quat& operator/=(const quat& q)
	{
		x /= q.x;
		y /= q.y;
		z /= q.z;
		w /= q.w;
		return *this;
	}

	quat& operator+=(float t)
	{
		x += t;
		y += t;
		z += t;
		w += t;
		return *this;
	}
	quat& operator-=(float t)
	{
		x -= t;
		y -= t;
		z -= t;
		w -= t;
		return *this;
	}
	quat& operator*=(float t)
	{
		x *= t;
		y *= t;
		z *= t;
		w *= t;
		return *this;
	}
	quat& operator/=(float t)
	{
		return *this *= (1 / t);
	}

	const quat operator+(const quat& q) const
	{
		return quat(*this) += q;
	}
	const quat operator-(const quat& q) const
	{
		return quat(*this) -= q;
	}
	const quat operator*(const quat& q) const
	{
		return quat(*this) *= q;
	}
	const quat operator/(const quat& q) const
	{
		return quat(*this) /= q;
	}

	const quat operator+(float t) const
	{
		return quat(*this) += t;
	}
	const quat operator-(float t) const
	{
		return quat(*this) -= t;
	}
	const quat operator*(float t) const
	{
		return quat(*this) *= t;
	}
	const quat operator/(float t) const
	{
		return quat(*this) *= (1 / t);
	}

	quat operator-()
	{
		return quat(*this) *= -1.0f;
	}
	bool operator==(const quat& q) const
	{
		return (x == q.x) && (y == q.y) && (z == q.z) && (w == q.w);
	}
	bool operator!=(const quat& q) const
	{
		return (x != q.x) || (y != q.y) || (z != q.z) || (w != q.w);
	}

	float dot(const quat& q) const
	{
		return x * q.x + y * q.y + z * q.z + w * q.w;
	}
	quat reflect(const quat& q) const
	{
		return q * (*this).dot(q) / q.length() / q.length() * 2 - *this;
	}
	void normalize()
	{
		float len = length();
		if (len > 0)
			*this /= len;
	}
	quat normalized() const
	{
		quat q(*this);
		q.normalize();
		return q;
	}
	float length() const
	{
		return sqrtl(x * x + y * y + z * z + w * w);
	}
};

const quat operator+(const float& t, const quat& q);
const quat operator-(const float& t, const quat& q);
const quat operator*(const float& t, const quat& q);
const quat operator/(const float& t, const quat& q);
