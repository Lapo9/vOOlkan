#ifndef VULKAN_HITBOX
#define VULKAN_HITBOX

#include <functional>

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

		virtual void onCollision() {
			onCollisionAction();
		}

		virtual void setCollisionAction(const std::function<void()>& action) {
			onCollisionAction = action;
		}

	protected:
		float scaleFactor;
		std::function<void()> onCollisionAction;

		Hitbox(Position position = {0.0f, 0.0f, 0.0f}, float scaleFactor = 1.0f, float mass = 1.0f, Speed initialSpeed = {0.0f, 0.0f, 0.0f}, Acceleration initialAcceleration = {0.0f, 0.0f, 0.0f}, Force internalForce = {0.0f, 0.0f, 0.0f}, Field emittedField = Field{ {0.0f, 0.0f, 0.0f}, FieldFunctions::emptyField }) :
			Cinematicable{ position, { 0.0f, 0.0f, 0.0f }, mass, initialSpeed, initialAcceleration, internalForce, emittedField }, scaleFactor{ scaleFactor }, onCollisionAction{ []() {} }
		{}
	};



	/**
	 * @brief A CircleHitbox is the Hitbox for circular-like objects. It is idendified by its center and radius.
	 */
	class CircleHitbox : public Hitbox {
	public:

		CircleHitbox(float radius, Position position = { 0.0f, 0.0f, 0.0f }, float scaleFactor = 1.0f, float mass = std::numeric_limits<float>::max()/10.0f, Speed initialSpeed = {0.0f, 0.0f, 0.0f}, Acceleration initialAcceleration = {0.0f, 0.0f, 0.0f}, Force internalForce = {0.0f, 0.0f, 0.0f}, Field emittedField = Field{{0.0f, 0.0f, 0.0f}, FieldFunctions::emptyField}) :
			Hitbox{ position, scaleFactor, mass, initialSpeed, initialAcceleration, internalForce, emittedField }
		{
			setRadius(radius);
		}


		float getRadius() const {
			return radius;
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



	class FrameHitbox : public Hitbox {
	public:

		//FIXTHIS std::numeric_limits<float>::max() / 10.0f this is an hack to deal with masses up to 10.0f
		template<std::same_as<Position>... P> requires (sizeof...(P) >= 2)
			FrameHitbox(Position position, float scaleFactor, P... frameVertices) : Hitbox{ position, scaleFactor, std::numeric_limits<float>::max() / 10.0f} {
			(vertices.push_back(frameVertices), ...);
		}


		const Segment& operator[](int i) const {
			auto v1 = rotation * glm::vec3(vertices[i]);
			auto v2 = rotation * glm::vec3(vertices[i + 1]);
			return Segment{ v1 + (position - Position{}), v2 + (position - Position{}) }; //returns the i-th segment in the "real" reference system
		}


		int getNumberOfSegments() const {
			return vertices.size() - 1;
		}


	private:
		std::vector<Position> vertices; //these vertices are considered in a reference system with origin in the center of this hitbox
	};

}


#endif
