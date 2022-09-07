#ifndef VULKAN_SEGMENT
#define VULKAN_SEGMENT

#include "Foundations.h"


namespace Vulkan::Physics {


	class Segment {
	public:
		Segment(Position p1, Position p2) : origin{ p1 }, direction{ p2 - p1 } {}

		float distance(Position p) const {
			auto distFromOrigin = p - origin; //distance of p from the origin of the segment
			auto distFromEnd = p - (origin + direction); //distance of p from the end of the segment

			auto projectionFromOrigin = distFromOrigin * direction;
			auto projectionFromEnd = distFromEnd * direction;

			//if the projection of the segment o-p is greater than the lenght of the segment, it means that the point is "outside" the segment, on the ending side, so the closest point of the segment to our point p is the end
			if (distFromOrigin > length()) {
				return glm::length(glm::vec3(p - (origin + direction)));
			}
			//if the projection of the segment e-p is greater than the lenght of the segment, it means that the point is "outside" the segment, on the starting side, so the closest point of the segment to our point p is the origin
			else if (distFromEnd > length()) {
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
