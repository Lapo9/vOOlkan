#ifndef VULKAN_MODEL
#define VULKAN_MODEL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <tuple>

#include "VertexInput.h"
#include "ModelLoader.h"
#include "Cinematicable.h"
#include "Foundations.h"


namespace Vulkan::Objects {

	template<typename V>
	concept IsVertex = requires(V v) {
		{V::getDescriptors(0)} -> std::same_as<std::pair<VkVertexInputBindingDescription, std::vector<VkVertexInputAttributeDescription>>>;
	};


	template<IsVertex Vertex, typename... Structs>
	class Model : public Physics::Cinematicable {

		using Matrices = struct {
			alignas(16) glm::mat4 mvp;
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 normals;
		};

	public:

		Model(Vertex, std::string pathToModel, Structs... uniforms) : vertices{}, indexes{}, uniforms{ Matrices{}, uniforms... } {
			ModelLoader<Vertex>::loadModel(pathToModel, vertices, indexes);
		}


		Model(Vertex v, std::string pathToModel, glm::vec3 rotation, glm::vec3 scale, glm::vec3 position, Structs... uniforms) : Model{ v, pathToModel, uniforms... } {
			setRotation(rotation);
			this->scaleFactor = scale;
			setPosition(position);
		}


		const std::vector<Vertex>& getVertices() const {
			return vertices;
		}


		const std::vector<uint32_t>& getIndexes() const {
			return indexes;
		}


		Model& rotate(float angle, glm::vec3 axis) override {
			rotation = glm::rotate(rotation, angle, axis);
			return *this;
		}


		Model& scale(glm::vec3 deltaFactors) {
			scale += deltaFactors;
			return *this;
		}


		Model& scale(float deltaFactor) {
			scale += glm::vec3{ deltaFactor, deltaFactor, deltaFactor };
			return *this;
		}


		const glm::mat4& calculateModelMatrix() const {
			return glm::translate(glm::mat4{ 1.0f }, glm::vec3(position)) *
				glm::scale(glm::mat4{ 1.0f }, scaleFactor) *
				glm::mat4{ rotation };
		}


		const glm::mat4& calculateNormalModelMatrix() const {
			return glm::inverse(glm::transpose(calculateModelMatrix()));
		}


		const glm::mat4& calculateMvpMatrix(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const {
			glm::mat4 modelMatrix = calculateModelMatrix();
			return projectionMatrix * viewMatrix * modelMatrix;
		}


		std::tuple<Matrices, Structs...>& getUniforms(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
			std::get<0>(uniforms).mvp = calculateMvpMatrix(viewMatrix, projectionMatrix);
			std::get<0>(uniforms).model = calculateModelMatrix();
			std::get<0>(uniforms).normals = calculateNormalModelMatrix();
			return uniforms;
		}


		template<unsigned int I>
		auto& getUniform(const glm::mat4& viewMatrix = glm::mat4{}, const glm::mat4& projectionMatrix = glm::mat4{}) {
			return std::get<I>(uniforms);
		}

		template<>
		auto& getUniform<0>(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
			std::get<0>(uniforms) = calculateMvpMatrix(viewMatrix, projectionMatrix);
			return std::get<0>(uniforms);
		}


		const Physics::Position& getPosition() const override {
			return position;
		}

		void setPosition(Physics::Position position) override {
			this->position = position;
		}


		glm::vec3& getScaleFactor() {
			return scaleFactor;
		}


		const glm::quat& getRotation() const override {
			return rotation;
		}

		void setRotation(glm::quat rotation) override {
			this->rotation = rotation;
		}


		void setMass(float mass) override {
			this->mass = mass;
		}

		float getMass() const override {
			return mass;
		}

		void setSpeed(Physics::Speed speed) override {
			this->speed = speed;
		}

		const Physics::Speed& getSpeed() const override {
			return speed;
		}

		void setAcceleration(Physics::Acceleration acceleration) override {
			this->acceleration = acceleration;
		}

		const Physics::Acceleration& getAcceleration() const override {
			return acceleration;
		}


	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indexes;
		std::tuple<Matrices, Structs...> uniforms;

		Physics::Position position;
		glm::quat rotation;
		glm::vec3 scaleFactor;

		float mass = 1;
		Physics::Speed speed;
		Physics::Acceleration acceleration;
	};
}

#endif
