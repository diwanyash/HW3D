#pragma once

#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;
constexpr float PI180 = PI / 180.0f;

template <typename T>
inline auto sq(const T& x)
{
	return x * x;
}

template<typename T>
inline T Interpolate(const T& src, const T& dest, const float alpha)
{
	return src + (dest - src) * alpha;
}

template<typename T>
inline T wrap_angle(T theta)
{
	const T modded = (T)fmod(theta, (T)2.0 * (T)PI_D);

	return (modded > (T)PI_D) ?
		(modded - (T)2.0 * (T)PI_D) :
		modded;
}

template<typename T>
constexpr inline T to_rad(T degree)
{
	return degree * PI180;
}
