
#include "vec3.h"

const vec3 operator+(const float& t, const vec3& v)
{
	return v + t;
}
const vec3 operator-(const float& t, const vec3& v)
{
	return v - t;
}
const vec3 operator*(const float& t, const vec3& v)
{
	return v * t;
}
const vec3 operator/(const float& t, const vec3& v)
{
	return v / t;
}


