#ifndef VULKAN_FOUNDATIONS
#define VULKAN_FOUNDATIONS


#include <glm/glm.hpp>


namespace Vulkan::Physics { class Position; class DeltaSpace; class Speed; class Acceleration; class Force; }


class Vulkan::Physics::Position {
public:
	Position(glm::vec3 position) : position{ position } {}

		
	friend DeltaSpace operator-(Position p1, Position p2);
	friend Position operator+(Position origin, DeltaSpace spaceCovered);
	friend Position& operator+=(Position& origin, DeltaSpace spaceCovered);

private:
	glm::vec3 position;
};




class Vulkan::Physics::DeltaSpace {
public:
	DeltaSpace(glm::vec3 deltaSpace) : deltaSpace{ deltaSpace } {}


	friend DeltaSpace operator+(DeltaSpace d1, DeltaSpace d2);
	friend DeltaSpace& operator+=(DeltaSpace& d1, DeltaSpace d2);
	friend DeltaSpace operator-(DeltaSpace d1, DeltaSpace d2);
	friend DeltaSpace& operator-=(DeltaSpace& d1, DeltaSpace d2);
	friend Position operator+(Position origin, DeltaSpace spaceCovered);
	friend Position& operator+=(Position& origin, DeltaSpace spaceCovered);

private:
	glm::vec3 deltaSpace;
};



class Vulkan::Physics::Speed {
public:
	Speed(glm::vec3 speed) : speed{ speed } {}


	friend Speed operator+(Speed s1, Speed s2);
	friend Speed& operator+=(Speed& s1, Speed s2);
	friend Speed operator-(Speed s1, Speed s2);
	friend Speed& operator-=(Speed& s1, Speed s2);
	friend DeltaSpace operator*(Speed speed, float time);

private:
	glm::vec3 speed;
};



class Vulkan::Physics::Acceleration {
public:
	Acceleration(glm::vec3 acceleration) : acceleration{ acceleration } {}


	friend Acceleration operator+(Acceleration s1, Acceleration s2);
	friend Acceleration& operator+=(Acceleration& s1, Acceleration s2);
	friend Acceleration operator-(Acceleration s1, Acceleration s2);
	friend Acceleration& operator-=(Acceleration& s1, Acceleration s2);
	friend Speed operator*(Acceleration speed, float time);

private:
	glm::vec3 acceleration;
};



class Vulkan::Physics::Force {
public:
	Force(glm::vec3 force) : force{ force } {}


	friend Force operator+(Force s1, Force s2);
	friend Force& operator+=(Force& s1, Force s2);
	friend Force operator-(Force s1, Force s2);
	friend Force& operator-=(Force& s1, Force s2);
	friend Acceleration operator*(Force force, float mass);

private:
	glm::vec3 force;
};



//operations
namespace Vulkan::Physics {
	
	DeltaSpace operator-(Position p1, Position p2) {
		return DeltaSpace{ p1.position - p2.position };
	}


	Position operator+(Position origin, DeltaSpace spaceCovered) {
		return Position{ origin.position + spaceCovered.deltaSpace };
	}


	Position& operator+=(Position& origin, DeltaSpace spaceCovered) {
		origin.position += spaceCovered.deltaSpace;
		return origin;
	}


	DeltaSpace operator+(DeltaSpace d1, DeltaSpace d2) {
		return DeltaSpace{ d1.deltaSpace + d2.deltaSpace };
	}


	DeltaSpace& operator+=(DeltaSpace& d1, DeltaSpace d2) {
		d1.deltaSpace += d2.deltaSpace;
		return d1;
	}


	DeltaSpace operator-(DeltaSpace d1, DeltaSpace d2) {
		return DeltaSpace{ d1.deltaSpace - d2.deltaSpace };
	}


	DeltaSpace& operator-=(DeltaSpace& d1, DeltaSpace d2) {
		d1.deltaSpace -= d2.deltaSpace;
		return d1;
	}


	Speed operator+(Speed s1, Speed s2) {
		return Speed{ s1.speed + s2.speed };
	}


	Speed& operator+=(Speed& s1, Speed s2) {
		s1.speed += s2.speed;
		return s1;
	}


	Speed operator-(Speed s1, Speed s2) {
		return Speed{ s1.speed - s2.speed };
	}


	Speed& operator-=(Speed& s1, Speed s2) {
		s1.speed -= s2.speed;
		return s1;
	}


	DeltaSpace operator*(Speed speed, float time) {
		return DeltaSpace{ speed.speed * time };
	}


	Acceleration operator+(Acceleration a1, Acceleration a2) {
		return Acceleration{ a1.acceleration + a2.acceleration };
	}


	Acceleration& operator+=(Acceleration& a1, Acceleration a2) {
		a1.acceleration += a2.acceleration;
		return a1;
	}


	Acceleration operator-(Acceleration a1, Acceleration a2) {
		return Acceleration{ a1.acceleration - a2.acceleration };
	}


	Acceleration& operator-=(Acceleration& a1, Acceleration a2) {
		a1.acceleration -= a2.acceleration;
		return a1;
	}


	Speed operator*(Acceleration acceleration, float time) {
		return Speed{ acceleration.acceleration * time };
	}


	Force operator+(Force f1, Force f2) {
		return Force{ f1.force + f2.force };
	}


	Force& operator+=(Force& f1, Force f2) {
		f1.force += f2.force;
		return f1;
	}


	Force operator-(Force f1, Force f2) {
		return Force{ f1.force - f2.force };
	}


	Force& operator-=(Force& f1, Force f2) {
		f1.force -= f2.force;
		return f1;
	}


	Acceleration operator*(Force force, float time) {
		return Acceleration{ force.force * time };
	}

}

#endif
