#ifndef VULKAN_ANIMATIONS
#define VULKAN_ANIMATIONS


#include "Pinball.h"

const float FLIPPER_ANGULAR_SPEED = 15.0f;
const float FLIPPER_MAX_ANGLE = 40.0_deg;
const float FLIPPER_MIN_ANGLE = -15.0_deg;
const float PULLER_MIN_Y = -6.700;
constexpr float PULLER_PULLUP_FORCE = 5000.0f;
const Vulkan::Physics::Position PULLER_RESTING_POSITION = Vulkan::Physics::Position{ 2.5f, -6.0f, 0.0f };


namespace Vulkan::Animations {

	template<Vulkan::Objects::IsVertex V, typename... S>
	void rightPadUp(Vulkan::Objects::Model<V, S...>& rightFlipper, int keyPressed) {
		if (keyPressed == GLFW_KEY_RIGHT || keyPressed == GLFW_KEY_D) {
			auto angle = (+rightFlipper).getRotationEuler()[0];
			if ((angle <= 0.0_deg && angle > -FLIPPER_MAX_ANGLE) || (angle > 0.0_deg && angle <= -(FLIPPER_MIN_ANGLE-0.1f))) {
				(+rightFlipper).setAngularSpeed(-FLIPPER_ANGULAR_SPEED);
			}
			else {
				(+rightFlipper).setAngularSpeed(0.0f);
			}
		}
	}


	template<Vulkan::Objects::IsVertex V, typename... S>
	void leftPadUp(Vulkan::Objects::Model<V, S...>& leftFlipper, int keyPressed) {
		if (keyPressed == GLFW_KEY_LEFT || keyPressed == GLFW_KEY_A) {
			auto angle = (+leftFlipper).getRotationEuler()[0];
			if ((angle < 0.0_deg && angle < -180.0_deg + FLIPPER_MAX_ANGLE) || (angle >0.0_deg && angle > 180.0_deg + (FLIPPER_MIN_ANGLE-0.1f))) {
				(+leftFlipper).setAngularSpeed(FLIPPER_ANGULAR_SPEED);
			}
			else {
				(+leftFlipper).setAngularSpeed(0.0f);
			}
		}
	}



	template<Vulkan::Objects::IsVertex V, typename... S>
	void pullerDown(Vulkan::Objects::Model<V, S...>& puller, int keyPressed) {
		if (keyPressed == GLFW_KEY_DOWN || keyPressed == GLFW_KEY_S) {
			if ((+puller).getPosition().y() > PULLER_MIN_Y) {
				(+puller).addExternalForce(Vulkan::Physics::Force{ 0.0f, -PULLER_PULLUP_FORCE - 0.00000001f, 0.0f });
			}
			else {
				(+puller).reset(Vulkan::Physics::Position{ PULLER_RESTING_POSITION.x(),PULLER_MIN_Y, PULLER_RESTING_POSITION.z() });
				(+puller).addExternalForce(Vulkan::Physics::Force{ 0.0f, -PULLER_PULLUP_FORCE, 0.0f });
			}
		}
	}

}


#endif
