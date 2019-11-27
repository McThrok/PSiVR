
#include "quat.h"

const quat operator+(const long double& t, const quat& q)
{
	return q + t;
}
const quat operator-(const long double& t, const quat& q)
{
	return q - t;
}
const quat operator*(const long double& t, const quat& q)
{
	return q * t;
}
const quat operator/(const long double& t, const quat& q)
{
	return q / t;
}