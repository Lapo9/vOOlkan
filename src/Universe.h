#ifndef VULKAN_UNIVERSE
#define VULKAN_UNIVERSE

#include <vector>
#include <variant>

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
			collisionDetection(elapsedSeconds);

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
		void collisionDetection(Time elapsedSeconds) {
			for (int i = 0; i < bodies.size(); ++i) {
				for (int j = i + 1; j < bodies.size(); ++j) {
					CircleHitbox& c1 = dynamic_cast<CircleHitbox&>(*bodies[i]);
					CircleHitbox& c2 = dynamic_cast<CircleHitbox&>(*bodies[j]);
					collisionDetection(c1, c2, elapsedSeconds);
				}
			}
		}


		void applyForces(Time elapsedSeconds) {
			for (auto body : bodies) {
				body->move(elapsedSeconds);
			}
		}




		static void collisionDetection(CircleHitbox& c1, CircleHitbox& c2, Time elapsedSeconds) {
			auto distance = c1.getPosition() - c2.getPosition();
			if ( distance <= c1.getRadius() + c2.getRadius()) {
				//get speeds and masses (to simplify the writing of the equation
				auto s1 = c1.getSpeed(); auto s2 = c2.getSpeed();
				auto m1 = c1.getMass(); auto m2 = c2.getMass();
				float e = 1.0f; //we simulate an elastic collision for now. This can vary based on materials.
				auto n = glm::normalize(glm::vec3(c1.getPosition() - c2.getPosition())); //versor pointing the direction between the 2 centers
				
				auto impulse = float((s2 - s1) * n * (-e - 1) * ((m1 * m2) / (m1 + m2)));

				c1.addExternalForce(-(impulse * n) / elapsedSeconds);
				c2.addExternalForce((impulse * n) / elapsedSeconds);
			}
		}




		std::vector<Hitbox*> bodies;
		std::vector<Field*> fields;

	};
}

#endif