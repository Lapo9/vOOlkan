#ifndef VULKAN_MOVEABLE
#define VULKAN_MOVEABLE

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Vulkan::Physics {

	class Moveable {
	public:
		virtual Moveable& translate(glm::vec3 delta) {
			setPosition(getPosition() + delta);
			return *this;
		};

		virtual const glm::vec3& getPosition() const = 0;
		virtual void setPosition(glm::vec3 position) = 0;

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
