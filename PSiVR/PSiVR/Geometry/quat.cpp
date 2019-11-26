
#include "quat.h"

const quat operator+(const float& t, const quat& q)
{
	return q + t;
}
const quat operator-(const float& t, const quat& q)
{
	return q - t;
}
const quat operator*(const float& t, const quat& q)
{
	return q * t;
}
const quat operator/(const float& t, const quat& q)
{
	return q / t;
}