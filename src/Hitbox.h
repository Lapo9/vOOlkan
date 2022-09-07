#ifndef VULKAN_HITBOX
#define VULKAN_HITBOX

#include "Cinematicable.h"
#include "Segment.h"


namespace Vulkan::Physics::FieldFunctions {
	Force emptyField(const Position&, const Cinematicable&);
}

namespace Vulkan::Physics {

	/**
	 * @brief A Hitbox is a Cinematicable object which can directly interact with other objects upon collision.
	 */
	class Hitbox : public Cinematicable {
	public:
		virtual float getScaleFactor() const {
			return scaleFactor;
		}

		virtual void setScaleFactor(float scaleFactor) {
			this->scaleFactor = scaleFactor;
		}

	protected:
		float scaleFactor;

		Hitbox(Position position = {0.0f, 0.0f, 0.0f}, float scaleFactor = 1.0f, float mass = 1.0f, Speed initialSpeed = {0.0f, 0.0f, 0.0f}, Acceleration initialAcceleration = {0.0f, 0.0f, 0.0f}, Force internalForce = {0.0f, 0.0f, 0.0f}, Field emittedField = Field{ {0.0f, 0.0f, 0.0f}, FieldFunctions::emptyField }) :
			Cinematicable{ position, { 0.0f, 0.0f, 0.0f }, mass, initialSpeed, initialAcceleration, internalForce, emittedField }, scaleFactor{ scaleFactor }
		{}
	};



	/**
	 * @brief A CircleHitbox is the Hitbox for circular-like objects. It is idendified by its center and radius.
	 */
	class CircleHitbox : public Hitbox {
	public:

		CircleHitbox(float radius, Position position = { 0.0f, 0.0f, 0.0f }, float scaleFactor = 1.0f, float mass = 1.0f, Speed initialSpeed = { 0.0f, 0.0f, 0.0f }, Acceleration initialAcceleration = { 0.0f, 0.0f, 0.0f }, Force internalForce = { 0.0f, 0.0f, 0.0f }, Field emittedField = Field{ {0.0f, 0.0f, 0.0f}, FieldFunctions::emptyField }) :
			Hitbox{ position, scaleFactor, mass, initialSpeed, initialAcceleration, internalForce, emittedField }
		{
			setRadius(radius);
		}


		float getRadius() const {
			return radius * this->scaleFactor;
		}

		void setRadius(float radius) {
			this->radius = radius * this->scaleFactor;
		}

		virtual void setScaleFactor(float scaleFactor) override {
			this->scaleFactor = scaleFactor;
			setRadius(radius);
		}

	private:
		float radius;
	
	};



	/**
	 * @brief A SquareHitbox is the Hitbox for rectangular-shaped objects. It is identified by its center, width and lenght.
	 */
	class SquareHitbox : public Hitbox {
	public:

		SquareHitbox(float width, float height, Position position = { 0.0f, 0.0f, 0.0f }, float scaleFactor = 1.0f, float mass = 1.0f, Speed initialSpeed = { 0.0f, 0.0f, 0.0f }, Acceleration initialAcceleration = { 0.0f, 0.0f, 0.0f }, Force internalForce = { 0.0f, 0.0f, 0.0f }, Field emittedField = Field{ {0.0f, 0.0f, 0.0f}, FieldFunctions::emptyField }) :
			Hitbox{ position, scaleFactor, mass, initialSpeed, initialAcceleration, internalForce, emittedField }
		{
			setWidthHeight(width, height);
		}


		std::pair<float, float> getWidthHeight() const {
			return { width * scaleFactor, height * scaleFactor };
		}

		void setWidthHeight(float width, float height) {
			this->width = width * scaleFactor;
			this->height = height * scaleFactor;
		}

		virtual void setScaleFactor(float scaleFactor) override {
			this->scaleFactor = scaleFactor;
			setWidthHeight( width, height );
		}

	private:
		float width;
		float height;

	};




	class FrameHitbox : public Hitbox {
	public:

		template<std::same_as<Position>... P> requires (sizeof...(P) >= 2)
			FrameHitbox(Position position, P... frameVertices) : Hitbox{ position } {
			(vertices.push_back(frameVertices), ...);
		}


		const Segment& operator[](int i) const {
			return Segment{ vertices[i] + (position - Position{}), vertices[i + 1] + (position - Position{}) }; //returns the i-th segment in the "real" reference system
		}


		int getNumberOfSegments() const {
			return vertices.size();
		}


	private:
		std::vector<Position> vertices; //these vertices are considered in a reference system with origin in the center of this hitbox
	};

}


#endif
