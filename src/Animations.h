#ifndef VULKAN_ANIMATIONS
#define VULKAN_ANIMATIONS


#include "Pinball.h"

const float FLIPPER_ANGULAR_SPEED = 15.0f;
const float FLIPPER_MAX_ANGLE = 40.0_deg;
const float FLIPPER_MIN_ANGLE = -15.0_deg;
const std::vector<Vulkan::Physics::Position> BALLS_RESTING_POSITIONS{ { 100.0f, 0.0f, 0.0f }, { 200.0f, 0.0f, 0.0f }, { 300.0f, 0.0f, 0.0f } };


namespace Vulkan::Animations {

	template<Vulkan::Objects::IsVertex V, typename... S>
	void rightPadUp(Vulkan::Objects::Model<V, S...>& rightFlipper, int keyPressed) {
		if (keyPressed == GLFW_KEY_RIGHT) {
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
		if (keyPressed == GLFW_KEY_LEFT) {
			auto angle = (+leftFlipper).getRotationEuler()[0];
			if ((angle < 0.0_deg && angle < -180.0_deg + FLIPPER_MAX_ANGLE) || (angle >0.0_deg && angle > 180.0_deg + (FLIPPER_MIN_ANGLE-0.1f))) {
				(+leftFlipper).setAngularSpeed(FLIPPER_ANGULAR_SPEED);
			}
			else {
				(+leftFlipper).setAngularSpeed(0.0f);
			}
		}
	}



}


#endif
