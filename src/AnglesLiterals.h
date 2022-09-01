#ifndef VULKAN_ANGLESLITERALS
#define VULKAN_ANGLESLITERALS

#include <numbers>

float operator"" _deg(long double degrees) {
	return degrees * std::numbers::pi / 180;
}

float operator"" _rad(long double radians) {
	return radians;
}

#endif
