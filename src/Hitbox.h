#ifndef VULKAN_HITBOX
#define VULKAN_HITBOX

#include "Cinematicable.h"


namespace Vulkan::Physics {

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

		Hitbox(Position position, float scaleFactor, float mass, Speed initialSpeed, Acceleration initialAcceleration, Force internalForce, Field emittedField) :
			Cinematicable{ position, { 0.0f, 0.0f, 0.0f }, mass, initialSpeed, initialAcceleration, internalForce, emittedField }, scaleFactor{ scaleFactor }
		{}
	};




	class CircleHitbox : public Hitbox {
	public:

		CircleHitbox(float radius, Position position = { 0.0f, 0.0f, 0.0f }, float scaleFactor = 1.0f, float mass = 1.0f, Speed initialSpeed = { 0.0f, 0.0f, 0.0f }, Acceleration initialAcceleration = { 0.0f, 0.0f, 0.0f }, Force internalForce = { 0.0f, 0.0f, 0.0f }, Field emittedField = Field{ {0.0f, 0.0f, 0.0f}, emptyField }) :
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




	class SquareHitbox : public Hitbox {
	public:

		SquareHitbox(float width, float height, Position position = { 0.0f, 0.0f, 0.0f }, float scaleFactor = 1.0f, float mass = 1.0f, Speed initialSpeed = { 0.0f, 0.0f, 0.0f }, Acceleration initialAcceleration = { 0.0f, 0.0f, 0.0f }, Force internalForce = { 0.0f, 0.0f, 0.0f }, Field emittedField = Field{ {0.0f, 0.0f, 0.0f}, emptyField }) :
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


}


#endif
