#ifndef VULKAN_FIELD
#define VULKAN_FIELD

#include "Foundations.h"


namespace Vulkan::Physics {
	class Cinematicable;

	/**
	 * @brief A force Field is an object which can interact with other objects with a force, defined by its calculateForce function.
	 */
	class Field {
	public:

		/**
		 * @brief A force Field is identified by its position and a function.
		 * 
		 * @param position The position of che "center" of the field.
		 * @param calculateForce A function which returns the force applied by the field to the object. Such force can be based on all of the characteristics of the object inside the field, such as its position, mass or speed. Therefore a field can also be used to compute forces like friction.
		 */
		Field(Position position, Force(*calculateForce)(const Position&, const Cinematicable&)) : position{ position }, calculateForce{ calculateForce } {

		}


		Position getPosition() const {
			return position;
		}


		void setPosition(Position position) {
			this->position = position;
		}


		/**
		 * @brief Returns the force applied by the field to a body.
		 * 
		 * @param body An object under the influx of this force field.
		 * @return The Force applied by the force field to the body.
		 */
		Force calculateAppliedForce(const Cinematicable& body) {
			return calculateForce(position, body);
		}


	private:
		Position position;
		std::function<Force(const Position&, const Cinematicable&)> calculateForce;
	};

}

#endif