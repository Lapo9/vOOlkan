#ifndef VULKAN_CINEMATICABLE
#define VULKAN_CINEMATICABLE

#include "Moveable.h"
#include "Foundations.h"
#include "Field.h"


namespace Vulkan::Physics::FieldFunctions {
	Force emptyField(const Position&, const Cinematicable&);
}


namespace Vulkan::Physics {
	
	/**
	 * @brief A Cinematicable object is a Movable object which obeys to physics laws.
	 * @details A Cinematicable object is determined by its position and rotation (since it is a Movable object) and its speed, acceleration.
	 *			A Cinematicable object can also emit a force Field, and can have an internal force (an engine like force).
	 */
	class Cinematicable : public Moveable {
	public:

		Cinematicable(Position position = { 0.0f, 0.0f, 0.0f }, glm::vec3 rotationEuler = { 0.0f, 0.0f, 0.0f }, Mass mass = 1.0f, Speed initialSpeed = { 0.0f, 0.0f, 0.0f }, Acceleration initialAcceleration = { 0.0f, 0.0f, 0.0f }, Force internalForce = { 0.0f, 0.0f, 0.0f }, Field emittedField = Field{ {0.0f, 0.0f, 0.0f}, FieldFunctions::emptyField }) :
			Moveable{ position, rotationEuler }, mass{ mass }, speed{ initialSpeed }, acceleration{ initialAcceleration }, internalForce{ internalForce }, emittedField{ emittedField } {}


		virtual void setMass(Mass mass) {
			this->mass = mass;
		}

		virtual Mass getMass() const {
			return mass;
		}

		virtual void setSpeed(Speed speed) {
			this->speed = speed;
		}

		virtual const Speed& getSpeed() const {
			return speed;
		}

		virtual void setAcceleration(Acceleration acceleration) {
			this->acceleration = acceleration;
		}

		virtual const Acceleration& getAcceleration() const {
			return acceleration;
		}

		virtual void setInternalForce(Force internalForce) {
			this->internalForce = internalForce;
		}

		virtual const Force& getInternalForce() const {
			return internalForce;
		}

		virtual Field getEmittedField() {
			return emittedField;
		}

		virtual void setPosition(Position position) override {
			DeltaSpace delta = position - this->position;
			this->position = position;
			emittedField.setPosition(emittedField.getPosition() + delta); //the field "follows" the object it is attached to
		}

		virtual Cinematicable& translate(DeltaSpace delta) override {
			setPosition(getPosition() + delta);
			return *this;
		};


		virtual float getAngularSpeed() const {
			return angularSpeed;
		}


		virtual void setAngularSpeed(float angularSpeed) {
			this->angularSpeed = angularSpeed;
		}


		/**
		 * @brief Sums to the already present external forces the argument.
		 * @details External forces are deleted once the move method is called, so they are basically impulse-like forces which act on the object in between 2 frames.
		 */
		virtual void addExternalForce(Force externalForce) {
			impulsiveForce += externalForce;
		}


		/**
		 * @brief Computes the new position of the object based on its current speed and the forces which are acting on the object.
		 */
		virtual void move(Time elapsedTime) {
			if (float(mass) == std::numeric_limits<float>::max()) {
				setAcceleration({ 0.0f, 0.0f, 0.0f });
			}
			else {
				setAcceleration((impulsiveForce + internalForce) / mass); // F = m*a (Newton III)
			}
			impulsiveForce = { 0.0f, 0.0f, 0.0f }; //reset impulsive forces
			setSpeed(speed + acceleration * elapsedTime); // ds = a/t --> s' = s + a/t
			translate(speed * elapsedTime); // dp = s/t --> p' = p + v/t
			rotate(angularSpeed * float(elapsedTime), glm::vec3(0.0f, 0.0f, 1.0f)); //only xy plane for now, and only arbitrary velocity (no angular acceleration based on momentum)
		}

	protected:
		Mass mass;
		Speed speed;
		Acceleration acceleration;
		Force internalForce;
		Force impulsiveForce; //externalForces which act on the body now. This is reset when move is called.
		Field emittedField;
		float angularSpeed; //only xy plane considered
	};

}


#endif
