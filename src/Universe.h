#ifndef VULKAN_UNIVERSE
#define VULKAN_UNIVERSE

#include <vector>
#include <variant>
#include <typeinfo>

#include "Hitbox.h"


namespace Vulkan::Physics {


	/**
	 * @brief A Universe is a container of Hitbox(es) and Field(s).
	 * @details The objects inside a universe can interact between them, by following basic dynamics laws.
	 */
	class Universe {
	public:

		/**
		 * @brief Builds an isolated Universe.
		 * 
		 * @param fields Vector containing all of the fields acting in this universe.
		 * @param ...hitboxes Objects which can interact among them and with the fields in the universe.
		 */
		template<typename... Hitboxes> requires (std::derived_from<Hitboxes, Hitbox> && ...)
			Universe(std::vector<Field*> fields, Hitboxes&... hitboxes) : fields{ fields } {
			(bodies.push_back(&hitboxes), ...);
		}



		/**
		 * @brief Calculates the new position of all the objects in the universe, their interaction with the other objects and the fields.
		 * 
		 * @param elapsedSeconds Seconds elapsed from last calculation. 2 sequences of calls to calculate with the same elapsedSeconds is deterministic, but 2 sequences with different elapsedSeconds is not.
		 */
		void calculate(float elapsedSeconds) {
			// 1. calculate forces for each object (i.e. fields)
			calculateFieldForces();

			// 2. collisions (detection and response)
			collisionDetection(elapsedSeconds);

			// 3. call move on each body
			applyForces(elapsedSeconds);
		}


		void addBody(Hitbox& body) {
			bodies.push_back(&body);
		}

		void removeBody(Hitbox& body) {
			bodies.erase(std::remove(bodies.begin(), bodies.end(), &body), bodies.end());
		}


	private:

		//Calculates the forces applied by the fields on the objects.
		void calculateFieldForces() {
			for (auto body : bodies) {
				for (auto field : fields) {
					body->addExternalForce(field->calculateAppliedForce(*body));
				}
			}
		}


		//Detects if there is any collision between 2 objects and in case resolves such collision.
		void collisionDetection(Time elapsedSeconds) {
			for (int i = 0; i < bodies.size(); ++i) {
				for (int j = i + 1; j < bodies.size(); ++j) {
					//if only C++ had multiple dynamic dispatch we wouldn't have to do this shit. Visitor is even worse.

					//circle - circle collision
					if (typeid(*bodies[i]) == typeid(CircleHitbox) && typeid(*bodies[j]) == typeid(CircleHitbox)) {
						CircleHitbox& c1 = static_cast<CircleHitbox&>(*bodies[i]);
						CircleHitbox& c2 = static_cast<CircleHitbox&>(*bodies[j]);
						collisionDetection(c2, c1, elapsedSeconds);
					}

					//frame - circle collision
					else if (typeid(*bodies[i]) == typeid(FrameHitbox) && typeid(*bodies[j]) == typeid(CircleHitbox)) {
						FrameHitbox& f1 = static_cast<FrameHitbox&>(*bodies[i]);
						CircleHitbox& c2 = static_cast<CircleHitbox&>(*bodies[j]);
						collisionDetection(f1, c2, elapsedSeconds);
					}	

					//circle - frame collision
					else if (typeid(*bodies[i]) == typeid(CircleHitbox) && typeid(*bodies[j]) == typeid(FrameHitbox)) {
						CircleHitbox& c1 = static_cast<CircleHitbox&>(*bodies[i]);
						FrameHitbox& f2 = static_cast<FrameHitbox&>(*bodies[j]);
						collisionDetection(f2, c1, elapsedSeconds);
					}
				}
			}
		}


		//Applies all of the forces calculated till now.
		void applyForces(Time elapsedSeconds) {
			for (auto body : bodies) {
				body->move(elapsedSeconds);
			}
		}



		//Collider between 2 circles
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

				c1.onCollision(c2);
				c2.onCollision(c1);
			}
		}


		static void collisionDetection(FrameHitbox& f, CircleHitbox& c, Time elapsedSeconds) {
			bool haveCollided = false; //turns true when at least one segment collided with the circle
			
			for (int i = 0; i < f.getNumberOfSegments(); ++i) {
				auto segment = f[i];
				if (segment.distance(c.getPosition()) <= c.getRadius()) {
					//get speeds and masses (to simplify the writing of the equation
					auto s1 = c.getSpeed(); auto s2 = f.getSpeed(); 
					auto m1 = c.getMass(); auto m2 = f.getMass();
					float e = 1.0f; //we simulate an elastic collision for now. This can vary based on materials.
					auto n = segment.normal(c.getPosition()); //normal to the segment, pointing to the segment

					//very simplified rotation dynamics
					auto sr2 = f.getAngularSpeed(); //in this simplified simulation, only frame hitboxes can rotate
					auto distanceFromCenterOfRotation = glm::length(glm::vec3((c.getPosition() + n * c.getRadius()) - f.getPosition())); //distance of the colliding point from the center of rotation

					auto tangentialSpeed = Speed(-segment.normal() * (sr2 * distanceFromCenterOfRotation)) * 2; //"linear" speed of the point of the frame that touched the circle
					auto massCoefficient = (m1 * m2) / (m1 + m2);
					auto impulse = float(((s2 + tangentialSpeed - s1) * n) * (-e - 1) * massCoefficient);

					f.addExternalForce((impulse * glm::vec3(n)) / elapsedSeconds); //rotational speed of the frame is not affected (simplification that can create problems)
					c.addExternalForce(-(impulse * glm::vec3(n)) / elapsedSeconds);

					haveCollided = true;
				}
			}

			if (haveCollided) {
				f.onCollision(c);
				c.onCollision(f);
			}
		}




		std::vector<Hitbox*> bodies;
		std::vector<Field*> fields;

	};
}

#endif