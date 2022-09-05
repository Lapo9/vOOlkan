#ifndef VULKAN_MOVEABLE
#define VULKAN_MOVEABLE

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Foundations.h"


namespace Vulkan::Physics {

	class Moveable {
	public:
		virtual Moveable& translate(DeltaSpace delta) {
			setPosition(getPosition() + delta);
			return *this;
		};

		virtual const Position& getPosition() const = 0;
		virtual void setPosition(Position position) = 0;

		virtual Moveable& rotate(float angle, glm::vec3 axis) {
			setRotation(glm::rotate(getRotation(), angle, axis));
			return *this;
		};

		virtual const glm::quat& getRotation() const = 0;
		virtual void setRotation(glm::quat rotation) = 0;

		virtual void setRotation(glm::vec3 eulerAngles) {
			setRotation(glm::quat(eulerAngles));
		};
	};
}

#endif
