
#include "vec3.h"

const vec3 operator+(const long double& t, const vec3& v)
{
	return v + t;
}
const vec3 operator-(const long double& t, const vec3& v)
{
	return v - t;
}
const vec3 operator*(const long double& t, const vec3& v)
{
	return v * t;
}
const vec3 operator/(const long double& t, const vec3& v)
{
	return v / t;
}


