#ifndef VULKAN_SEGMENT
#define VULKAN_SEGMENT

#include "Foundations.h"


namespace Vulkan::Physics {


	class Segment {
	public:
		Segment(Position p1, Position p2) : origin{ p1 }, direction{ p2 - p1 } {}

		float distance(Position p) const {
			auto distFromOrigin = p - origin; //distance of p from the origin of the segment

			auto projectionFromOrigin = (distFromOrigin * direction) / length();

			//if the projection of the segment o-p is greater than the lenght of the segment, it means that the point is "outside" the segment, on the ending side, so the closest point of the segment to our point p is the end
			if (projectionFromOrigin > length()) {
				return glm::length(glm::vec3(p - (origin + direction)));
			}
			//if the projection of the segment o-p is smaller than 0, it means that the point is "outside" the segment, on the starting side, so the closest point of the segment to our point p is the origin
			else if (projectionFromOrigin < 0.0f) {
				return glm::length(glm::vec3(p - origin));
			}
			//else the closest point is one of the points internal to the segment
			else {
				auto closestPoint = origin + glm::normalize(glm::vec3(direction)) * projectionFromOrigin;
				return glm::length(glm::vec3(p - closestPoint));
			}
		}


		float length() const {
			return glm::length(glm::vec3(direction));
		}

		//TODO not efficient
		/**
		 * @brief Returns the normal always pointing to the vector.
		 */
		DeltaSpace normal(Position p) const {
			auto distFromOrigin = p - origin; //distance of p from the origin of the segment
			auto projectionFromOrigin = (distFromOrigin * direction) / length();
			auto closestPoint = origin + glm::normalize(glm::vec3(direction)) * projectionFromOrigin; //calculate the closest point of the segment (or its prolongment) to the point
			return glm::normalize(glm::vec3(closestPoint - p)); //the normal pointing to the segment
		}


		/**
		 * @brief Returns the clockwise normal.
		 */
		DeltaSpace normal() const {
			return glm::vec3{ direction.y(), -direction.x(), direction.z()};
		}

		const Position& getOrigin() const {
			return origin;
		}

		const DeltaSpace& getDirection() const {
			return direction;
		}

	private:
		Position origin;
		DeltaSpace direction;
	};


}


#endif
