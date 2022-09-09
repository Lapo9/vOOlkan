#ifndef VULKAN_MOVEABLE
#define VULKAN_MOVEABLE

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Foundations.h"


namespace Vulkan::Physics {

	/**
	 * @brief A Movable object is an object which can be moved and rotated.
	 */
	class Moveable {
	public:
		Moveable(Position position = { 0.0f, 0.0f, 0.0f }, glm::vec3 rotationEuler = { 0.0f, 0.0f, 0.0f }) : position{ position } {
			setRotation(rotationEuler);
		}

		virtual Moveable& translate(DeltaSpace delta) {
			setPosition(getPosition() + delta);
			return *this;
		};

		virtual const Position& getPosition() const {
			return position;
		}

		virtual void setPosition(Position position) {
			this->position = position;
		}

		virtual Moveable& rotate(float angle, glm::vec3 axis) {
			setRotation(glm::rotate(getRotation(), angle, axis));
			return *this;
		};

		virtual const glm::quat& getRotation() const {
			return rotation;
		}

		virtual const glm::vec3& getRotationEuler() const {
			auto& q = rotation;

			auto yaw = glm::atan(2 * (q[0] * q[1] + q[2] * q[3]), 1 - 2 * (q[1] * q[1] + q[2] * q[2]));
			auto pitch = glm::asin(2 * (q[0] * q[2] - q[3] * q[1]));
			auto roll = glm::atan(2 * (q[0] * q[3] + q[1] * q[2]), 1 - 2 * (q[2] * q[2] + q[3] * q[3]));
		
			return { yaw, pitch, roll };
		}

		virtual void setRotation(glm::quat rotation) {
			this->rotation = rotation;
		}

		virtual void setRotation(glm::vec3 eulerAngles) {
			setRotation(glm::quat(eulerAngles));
		};


	protected:
		Position position;
		glm::quat rotation;
	};
}

#endif
