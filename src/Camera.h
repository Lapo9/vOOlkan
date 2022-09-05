#ifndef VULKAN_CAMERA
#define VULKAN_CAMERA

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Moveable.h"
#include "Foundations.h"


namespace Vulkan::Objects {


	class Camera : public Physics::Moveable {
	public:

		Camera(glm::vec3 position, glm::vec3 rotationEuler) {
			setRotation(rotationEuler);
		}


		const Physics::Position& getPosition() const override {
			return position;
		}

		void setPosition(Physics::Position position) override {
			this->position = position;
		}

		const glm::quat& getRotation() const override {
			return rotation;
		}

		void setRotation(glm::quat rotation) override {
			this->rotation = rotation;
		}


		glm::mat4 getViewMatrix() const {
			return glm::rotate(glm::mat4(1.0f), -rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
				glm::rotate(glm::mat4(1.0f), -rotation.y, glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), -rotation.x, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::translate(glm::mat4(1.0f), glm::vec3(-position));
		}


	private:
		Physics::Position position;
		glm::quat rotation;

	};

}


#endif
