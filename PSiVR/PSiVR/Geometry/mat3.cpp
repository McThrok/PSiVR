#include "mat3.h"

const mat3 operator+(const float& t, const mat3& m)
{
	return m + t;
}
const mat3 operator-(const float& t, const mat3& m)
{
	return m - t;
}
const mat3 operator*(const float& t, const mat3& m)
{
	return m * t;
}
const mat3 operator/(const float& t, const mat3& m)
{
	return m / t;
}