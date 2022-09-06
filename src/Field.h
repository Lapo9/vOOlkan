#ifndef VULKAN_FIELD
#define VULKAN_FIELD

#include "Foundations.h"



namespace Vulkan::Physics {
	class Cinematicable;

	class Field {
	public:

		Field(Position position, Force(*calculateForce)(const Position&, const Cinematicable&)) : position{ position }, calculateForce{ calculateForce } {

		}


		Position getPosition() const {
			return position;
		}


		void setPosition(Position position) {
			this->position = position;
		}


		Force calculateAppliedForce(const Cinematicable& body) {
			return calculateForce(position, body);
		}


	private:
		Position position;
		std::function<Force(const Position&, const Cinematicable&)> calculateForce;
	};


	Force emptyField(const Position&, const Cinematicable&) {
		return Force{ 0.0f, 0.0f, 0.0f };
	}

}

#endif