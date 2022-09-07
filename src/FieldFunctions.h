#ifndef VULKAN_FIELDFUNCTIONS
#define VULKAN_FIELDFUNCTIONS

#include "Field.h"
#include "Cinematicable.h"
#include "Foundations.h"


namespace Vulkan::Physics::FieldFunctions {
	Force emptyField(const Position&, const Cinematicable&) {
		return Force{ 0.0f, 0.0f, 0.0f };
	}


	template<float intensity>
	Force centralField(const Position& fieldCenter, const Cinematicable& body) {
		auto dir = glm::normalize(glm::vec3(fieldCenter - body.getPosition()));
		return dir * intensity;
	}


	template<float mu>
	Force friction(const Position& fieldCenter, const Cinematicable& body) {
		auto dir = glm::vec3(-body.getSpeed());
		return dir * mu;
	}
}

#endif
