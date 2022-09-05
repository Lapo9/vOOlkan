#ifndef VULKAN_FIELD
#define VULKAN_FIELD

#include "Foundations.h"


namespace Vulkan::Physics {

	template<typename... Args>
	class Field {
	public:

		Field(Position position, Force(*calculateForce)(Position, Args...)) : body{}, calculateForce{ calculateForce } {
			body.setPosition(position);
		}


		auto& getCinematicable() {
			return body;
		}


		Force calculateAppliedForce(Args... args) {
			return calculateForce(body.getPosition(), args...);
		}

	private:
		class : public Cinematicable {
		public:
			const Position& getPosition() const override {
				return position;
			}

			void setPosition(Position position) override {
				this->position = position;
			}

			const glm::quat& getRotation() const override {
				return rotation;
			}

			void setRotation(glm::quat rotation) override {
				this->rotation = rotation;
			}

			float getMass() const override {
				return mass;
			}

			void setMass(float mass) override {
				this->mass = mass;
			}

			const Speed& getSpeed() const override {
				return speed;
			}

			void setSpeed(Speed speed) override {
				this->speed = speed;
			}

			const Acceleration& getAcceleration() const override {
				return acceleration;
			}

			void setAcceleration(Acceleration acceleration) {
				this->acceleration = acceleration;
			}


		private:
			Position position;
			glm::quat rotation;
			float mass = 1;
			Speed speed;
			Acceleration acceleration;

		} body;

		std::function<Force(Position, Args...)> calculateForce;
	};

}

#endif