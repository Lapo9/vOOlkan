#ifndef VULKAN_CINEMATICABLE
#define VULKAN_CINEMATICABLE

#include "Moveable.h"
#include "Foundations.h"


namespace Vulkan::Physics {

	class Cinematicable : public Moveable {
	public:

		virtual void setMass(float mass) = 0;
		virtual float getMass() = 0;

		virtual void setSpeed(Speed speed) = 0;
		virtual const Speed& getSpeed() = 0;

		virtual void setAcceleration(Acceleration acceleration) = 0;
		virtual const Acceleration& getAcceleration() = 0;


		virtual void move(float elapsedTime, Force totalForce) {
			setAcceleration(totalForce / getMass());
			setSpeed(getSpeed() + getAcceleration() * elapsedTime);
			setPosition(getPosition() + getSpeed() * elapsedTime);
		}
	};

}


#endif
