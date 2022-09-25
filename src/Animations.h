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

	//These 2 functions are used to check whether the pad is in his "working area" without using the angles. This is because it is hard to obtain the angles from the quaternion.
	bool checkRightPadArea(const Vulkan::Physics::FrameHitbox& rightFlipper, float slackMax = 0.0f, float slackMin = 0.0f) {
		const auto& segment = rightFlipper[0]; //the segment of the hitbox of the pad
		const auto& extreme = segment.getDirection(); //the extreme point of the segment if its origin was on (0; 0)
		const auto maxX = glm::cos(180.0_deg - FLIPPER_MAX_ANGLE) * segment.length(); //the maximum x coordinate of the extreme of the segment if it goes "over" this value it goes out of the "working area".
		const auto minX = glm::cos(-180.0_deg - FLIPPER_MIN_ANGLE) * segment.length(); //the minumum x coordinate of the extreme of the segment if it goes "under" this value it goes out of the "working area".

		//check the extremes (instead of the angle)
		return extreme.x() < maxX + slackMax && extreme.y() > 0.0f || extreme.x() < minX + slackMin && extreme.y() <= 0.0f;
	}

	bool checkLeftPadArea(const Vulkan::Physics::FrameHitbox& leftFlipper, float slackMax = 0.0f, float slackMin = 0.0f) {
		//look at previous function for explaination
		const auto& segment = leftFlipper[0];
		const auto& extreme = segment.getDirection();
		const auto maxX = glm::cos(FLIPPER_MAX_ANGLE) * segment.length();
		const auto minX = glm::cos(FLIPPER_MIN_ANGLE) * segment.length();

		return extreme.x() > maxX - slackMax && extreme.y() > 0.0f || extreme.x() > minX - slackMin && extreme.y() <= 0.0f;
	}



	template<Vulkan::Objects::IsVertex V, typename... S>
	void rightPadUp(Vulkan::Objects::Model<V, S...>& rightFlipper, int keyPressed) {
		if (keyPressed == GLFW_KEY_RIGHT || keyPressed == GLFW_KEY_D) {
			if (checkRightPadArea(static_cast<Vulkan::Physics::FrameHitbox&>(+rightFlipper), 0.0f, 0.1f)) {
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
			if (checkLeftPadArea(static_cast<Vulkan::Physics::FrameHitbox&>(+leftFlipper), 0.0f, 0.1f)) {
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
				(+puller).addExternalForce(Vulkan::Physics::Force{ 0.0f, -PULLER_PULLUP_FORCE - 0.1f, 0.0f });
			}
			else {
				(+puller).reset(Vulkan::Physics::Position{ PULLER_RESTING_POSITION.x(),PULLER_MIN_Y, PULLER_RESTING_POSITION.z() });
				(+puller).addExternalForce(Vulkan::Physics::Force{ 0.0f, -PULLER_PULLUP_FORCE, 0.0f });
			}
		}
	}

}


#endif
