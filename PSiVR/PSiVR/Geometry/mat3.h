#pragma once

#include <math.h>

#include "vec3.h"

class mat3
{
public:
	long double m[3][3];

	mat3()
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m[i][j] = 0;
	}
	mat3(long double m11, long double m12, long double m13, long double m21, long double m22, long double m23, long double m31, long double m32, long double m33)
	{
		m[0][0] = m11;
		m[0][1] = m12;
		m[0][2] = m13;
		m[1][0] = m21;
		m[1][1] = m22;
		m[1][2] = m23;
		m[2][0] = m31;
		m[2][1] = m32;
		m[2][2] = m33;
	}
	mat3(const mat3& mat)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m[i][j] = mat.m[i][j];
	}

	mat3& operator=(const mat3& mat)
	{
		if (this != &mat)
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					m[i][j] = mat.m[i][j];
		}
		return *this;
	}

	mat3 inv() {
		//errors
		mat3 mat(*this);
		int i, j;
		long double determinant = 0;

		//finding determinant
		for (i = 0; i < 3; i++)
			determinant = determinant + (mat.m[0][i] * (mat.m[1][(i + 1) % 3] * mat.m[2][(i + 2) % 3] - mat.m[1][(i + 2) % 3] * mat.m[2][(i + 1) % 3]));


		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				mat.m[i][j] = ((mat.m[(j + 1) % 3][(i + 1) % 3] * mat.m[(j + 2) % 3][(i + 2) % 3]) - (mat.m[(j + 1) % 3][(i + 2) % 3] * mat.m[(j + 2) % 3][(i + 1) % 3])) / determinant;

		return mat;
	}
	mat3 identity() {
		mat3 mat;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				mat.m[i][j] = 0;

		return mat;
	}
	vec3 transform(const vec3& v)
	{
		return vec3(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
		);
	}

	mat3 transposed() const
	{
		mat3 mat(*this);
		mat.transpose();
		return mat;
	}
	void transpose()
	{
		long double tmp = m[0][1];
		m[0][1] = m[1][0];
		m[1][0] = tmp;

		tmp = m[0][2];
		m[0][2] = m[2][0];
		m[2][0] = tmp;

		tmp = m[1][2];
		m[1][2] = m[2][1];
		m[2][1] = tmp;
	}

	mat3 createScale(long double x, long double y, long double z)
	{
		mat3 scale;
		scale.m[0][0] = x;
		scale.m[1][1] = y;
		scale.m[2][2] = z;

		return scale;
	}
	mat3 createScale(vec3 s)
	{
		return createScale(s.x, s.y, s.z);
	}
	mat3 createScale(long double s)
	{
		return createScale(s, s, s);
	}

	mat3 createRotationX(long double rad)
	{
		mat3 scale;
		scale.m[0][0] = 1;
		scale.m[1][1] = cosl(rad);
		scale.m[1][2] = -sinl(rad);
		scale.m[2][1] = sinl(rad);
		scale.m[2][2] = cosl(rad);

		return scale;
	}
	mat3 createRotationY(long double rad)
	{
		mat3 scale;
		scale.m[1][1] = 1;
		scale.m[0][0] = cosl(rad);
		scale.m[0][2] = sinl(rad);
		scale.m[2][0] = -sinl(rad);
		scale.m[2][2] = cosl(rad);

		return scale;
	}
	mat3 createRotationZ(long double rad)
	{
		mat3 scale;
		scale.m[2][2] = 1;
		scale.m[0][0] = cosl(rad);
		scale.m[0][1] = -sinl(rad);
		scale.m[1][0] = sinl(rad);
		scale.m[1][1] = cosl(rad);

		return scale;
	}

	mat3& operator+=(const mat3& mat)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m[i][j] += mat.m[i][j];
		return *this;
	}
	mat3& operator-=(const mat3& mat)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m[i][j] -= mat.m[i][j];
		return *this;
	}
	mat3& operator*=(const mat3& mat)
	{
		*this = *this * mat;
		return *this;
	}
	mat3& operator+=(long double t)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m[i][j] += t;
		return *this;
	}
	mat3& operator-=(long double t)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m[i][j] -= t;
		return *this;
	}
	mat3& operator*=(long double t)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				m[i][j] *= t;
		return *this;
	}
	mat3& operator/=(long double t)
	{
		return *this *= (1 / t);
	}
	const mat3 operator+(const mat3& mat) const
	{
		return mat3(*this) += mat;
	}
	const mat3 operator-(const mat3& mat) const
	{
		return mat3(*this) -= mat;
	}
	/*!*/const mat3 operator*(const mat3& mat) const
	{
		mat3 result;
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				for (int k = 0; k < 3; ++k)
					result.m[i][j] += m[i][k] * mat.m[k][j];
		return result;
	}
	const mat3 operator+(long double t) const
	{
		return mat3(*this) += t;
	}
	const mat3 operator-(long double t) const
	{
		return mat3(*this) -= t;
	}
	const mat3 operator*(long double t) const
	{
		return mat3(*this) *= t;
	}
	const mat3 operator/(long double t) const
	{
		return mat3(*this) *= (1 / t);
	}
	mat3 operator-()
	{
		return mat3(*this) *= -1.0f;
	}
	bool operator==(const mat3& mat) const
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				if (m[i][j] != mat.m[i][j])
					return false;
		return true;
	}
	bool operator!=(const mat3& mat) const
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				if (m[i][j] != mat.m[i][j])
					return true;
		return false;
	}
};

const mat3 operator+(const long double& t, const mat3& m);
const mat3 operator-(const long double& t, const mat3& m);
const mat3 operator*(const long double& t, const mat3& m);
const mat3 operator/(const long double& t, const mat3& m);
