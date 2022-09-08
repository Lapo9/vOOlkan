#ifndef VULKAN_FOUNDATIONS
#define VULKAN_FOUNDATIONS


#include <glm/glm.hpp>
#include <functional>


namespace Vulkan::Physics { class Vectorial; class Scalar; class Position; class DeltaSpace; class Speed; class Acceleration; class Force; class Impulse; class Mass; class Time; }


class Vulkan::Physics::Vectorial {
public:
	Vectorial(glm::vec3 vector = { 0.0f, 0.0f, 0.0f }) : vector{ vector } {}

	Vectorial(float x, float y, float z) : vector{ x,y,z } {}



	float x() const{
		return vector.x;
	}

	float y() const {
		return vector.y;
	}

	float z() const {
		return vector.z;
	}



	operator glm::vec3() const {
		return vector;
	}



	template<std::derived_from<Vectorial> V>
	friend V operator+(V v1, V v2) {
		return V{ v1.vector + v2.vector };
	}


	template<std::derived_from<Vectorial> V>
	friend V operator-(V v1, V v2) {
		return V{ v1.vector - v2.vector };
	}


	template<std::derived_from<Vectorial> V>
	friend V operator-(V v) {
		return V{ -v.vector };
	}


	template<std::derived_from<Vectorial> V>
	friend V& operator+=(V& v1, V v2) {
		v1.vector += v2.vector;
		return v1;
	}


	template<std::derived_from<Vectorial> V>
	friend V& operator-=(V& v1, V v2) {
		v1.vector -= v2.vector;
		return v1;
	}


	template<std::derived_from<Vectorial> V>
	friend V operator*(V v, float x) {
		return V{ v.vector * x };
	}


	template<std::derived_from<Vectorial> V>
	friend V operator/(V v, float x) {
		return V{ v.vector / x };
	}


	friend float operator*(Vectorial v1, Vectorial v2) {
		return glm::dot(v1.vector, v2.vector);
	}


	template<std::derived_from<Vectorial> V>
	friend auto operator<=>(const V& v1, float v2) {
		if (glm::length(v1.vector) < v2) {
			return -1;
		}
		else if (glm::length(v1.vector) > v2) {
			return 1;
		}
		else {
			return 0;
		}
	}


	template<std::derived_from<Vectorial> V>
	friend auto operator<=>(const V& v1, const V& v2) {
		if (glm::length(v1.vector) < glm::length(v2.vector)) {
			return -1;
		}
		else if (glm::length(v1.vector) > glm::length(v2.vector)) {
			return 1;
		}
		else {
			return 0;
		}
	}

	


protected:
	glm::vec3 vector;
};


class Vulkan::Physics::Scalar {
public:
	Scalar(float value = 0) : value{ value } {}

	template<std::derived_from<Scalar> S>
	friend S operator+(S s1, S s2) {
		return S{ s1.value + s2.value };
	}

	template<std::derived_from<Scalar> S>
	friend S operator-(S s1, S s2) {
		return S{ s1.value - s2.value };
	}

	template<std::derived_from<Scalar> S>
	friend S& operator+=(S& s1, S s2) {
		s1.value += s2.value;
		return s1;
	}

	template<std::derived_from<Scalar> S>
	friend S& operator-=(S& s1, S s2) {
		s1.value -= s2.value;
		return s1;
	}


	explicit operator float() const {
		return value;
	}


protected:
	float value;
};


class Vulkan::Physics::Mass : public Vulkan::Physics::Scalar {
public:
	Mass(float mass = 0) : Scalar{ mass }{}

	friend Mass operator*(Mass m1, Mass m2); //not correct but it works for now
	friend Mass operator/(Mass m1, Mass m2); //not correct but it works for now
	friend Impulse operator*(Speed speed, Mass mass);
	friend Speed operator/(Impulse impulse, Mass mass);
	friend Acceleration operator/(Force force, Mass mass);
};




class Vulkan::Physics::Time : public Vulkan::Physics::Scalar {
public:
	Time(float time) : Scalar{ time } {}

	friend DeltaSpace operator*(Speed speed, Time time);
	friend Speed operator*(Acceleration speed, Time time);
	friend Force operator/(Impulse impulse, Time time);
};




class Vulkan::Physics::Position {
public:
	Position(glm::vec3 position = { 0.0f, 0.0f, 0.0f }) : position{ position } {}

	Position(float x, float y, float z) : Position{ glm::vec3{x, y, z} } {}


	float& x() {
		return position.x;
	}

	float& y() {
		return position.y;
	}

	float& z() {
		return position.z;
	}


	operator glm::vec3() const {
		return position;
	}
		
	friend DeltaSpace operator-(Position p1, Position p2);
	friend Position operator+(Position origin, DeltaSpace spaceCovered);
	friend Position& operator+=(Position& origin, DeltaSpace spaceCovered);

	Position operator-() const {
		return Position{ -position };
	}

	friend auto operator==(const Position& p1, const Position& p2) {
		return p1.position == p2.position;
	}

private:
	glm::vec3 position;
};




class Vulkan::Physics::DeltaSpace : public Vulkan::Physics::Vectorial {
public:
	DeltaSpace(glm::vec3 vector = { 0.0f, 0.0f, 0.0f }) : Vectorial{ vector } {}
	DeltaSpace(float x, float y, float z) : Vectorial{ x,y,z } {}

	friend Position operator+(Position origin, DeltaSpace spaceCovered);
	friend Position& operator+=(Position& origin, DeltaSpace spaceCovered);
};




class Vulkan::Physics::Speed : public Vulkan::Physics::Vectorial {
public:
	Speed(glm::vec3 vector = { 0.0f, 0.0f, 0.0f }) : Vectorial{ vector } {}
	Speed(float x, float y, float z) : Vectorial{ x,y,z } {}

	friend DeltaSpace operator*(Speed speed, Time time);
	friend Impulse operator*(Speed speed, Mass mass);
};




class Vulkan::Physics::Acceleration : public Vulkan::Physics::Vectorial {
public:
	Acceleration(glm::vec3 vector = { 0.0f, 0.0f, 0.0f }) : Vectorial{ vector } {}
	Acceleration(float x, float y, float z) : Vectorial{ x,y,z } {}

	friend Speed operator*(Acceleration speed, Time time);
};




class Vulkan::Physics::Force : public Vulkan::Physics::Vectorial {
public:
	Force(glm::vec3 vector = { 0.0f, 0.0f, 0.0f }) : Vectorial{ vector } {}
	Force(float x, float y, float z) : Vectorial{ x,y,z } {}

	friend Acceleration operator/(Force force, Mass mass);
};




class Vulkan::Physics::Impulse : public Vulkan::Physics::Vectorial {
public:
	Impulse(glm::vec3 vector = { 0.0f, 0.0f, 0.0f }) : Vectorial{ vector } {}
	Impulse(float x, float y, float z) : Vectorial{ x,y,z } {}

	friend Speed operator/(Impulse impulse, Mass mass);
	friend Force operator/(Impulse impulse, Time time);
};





//operations
namespace Vulkan::Physics {

	Mass operator*(Mass m1, Mass m2) {
		return Mass{ m1.value * m2.value };
	}

	Mass operator/(Mass m1, Mass m2) {
		return Mass{ m1.value / m2.value };
	}


	
	DeltaSpace operator-(Position p1, Position p2) {
		return DeltaSpace{ p1.position - p2.position };
	}


	Position operator+(Position origin, DeltaSpace spaceCovered) {
		return Position{ origin.position + spaceCovered.vector };
	}


	Position& operator+=(Position& origin, DeltaSpace spaceCovered) {
		origin.position += spaceCovered.vector;
		return origin;
	}




	DeltaSpace operator*(Speed speed, Time time) {
		return DeltaSpace{ speed.vector * time.value };
	}


	Impulse operator*(Speed speed, Mass mass) {
		return Impulse{ speed.vector * mass.value };
	}




	Speed operator*(Acceleration acceleration, Time time) {
		return Speed{ acceleration.vector * time.value };
	}




	Acceleration operator/(Force force, Mass mass) {
		return Acceleration{ force.vector / mass.value };
	}



	Speed operator/(Impulse impulse, Mass mass) {
		return Speed{ impulse.vector / mass.value };
	}


	Force operator/(Impulse impulse, Time time) {
		return Force{ impulse.vector / time.value };
	}



}

#endif
