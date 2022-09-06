#ifndef VULKAN_UNIVERSE
#define VULKAN_UNIVERSE

#include <vector>

#include "Hitbox.h"


namespace Vulkan::Physics {


	class Universe {
	public:

		template<typename... Hitboxes> requires (std::derived_from<Hitboxes, Hitbox> && ...)
			Universe(std::vector<Field*> fields, Hitboxes&... hitboxes) : fields{ fields } {
			(bodies.push_back(&hitboxes), ...);
		}



		void calculate(float elapsedSeconds) {
			// 1. calculate forces for each object (i.e. fields)
			calculateForces();

			// 2. collisions (detection and response)
			collisionDetection();

			// 3. call move on each body
			applyForces(elapsedSeconds);
		}




	private:

		void calculateForces() {
			for (auto body : bodies) {
				for (auto field : fields) {
					body->addExternalForce(field->calculateAppliedForce(*body));
				}
			}
		}


		//FROMHERE
		void collisionDetection() {
			
		}


		void applyForces(float elapsedSeconds) {
			for (auto body : bodies) {
				body->move(elapsedSeconds);
			}
		}



		std::vector<Hitbox*> bodies;
		std::vector<Field*> fields;

	};
}

#endif