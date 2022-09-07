#ifndef VULKAN_CINEMATICABLE
#define VULKAN_CINEMATICABLE

#include "Moveable.h"
#include "Foundations.h"
#include "Field.h"


namespace Vulkan::Physics::FieldFunctions {
	Force emptyField(const Position&, const Cinematicable&);
}


namespace Vulkan::Physics {

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


		virtual void addExternalForce(Force externalForce) {
			impulsiveForce += externalForce;
		}


		virtual void move(Time elapsedTime) {
			setAcceleration((impulsiveForce + internalForce) / getMass());
			impulsiveForce = { 0.0f, 0.0f, 0.0f }; //reset impulsive forces
			setSpeed(getSpeed() + getAcceleration() * elapsedTime);
			setPosition(getPosition() + getSpeed() * elapsedTime);
		}

	protected:
		Mass mass;
		Speed speed;
		Acceleration acceleration;
		Force internalForce;
		Force impulsiveForce; //externalForces which act on the body now. This is reset when move is called
		Field emittedField;

	};

}


#endif
