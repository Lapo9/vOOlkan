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
#include "Hitbox.h"
#include "KeyboardController.h"


namespace Vulkan::Objects {

	template<typename V>
	concept IsVertex = requires(V v) {
		{V::getDescriptors(0)} -> std::same_as<std::pair<VkVertexInputBindingDescription, std::vector<VkVertexInputAttributeDescription>>>;
	};


	template<IsVertex Vertex, typename... Structs>
	class Model : public Utilities::KeyboardListener {

		using Matrices = struct {
			alignas(16) glm::mat4 mvp;
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 normals;
		};

	public:

		Model(std::unique_ptr<Vulkan::Physics::Hitbox> hitbox, glm::vec3 rotationEuler, Vertex, std::string pathToModel, Structs... uniforms) : vertices{}, indexes{}, uniforms{ Matrices{}, uniforms... }, hitbox{ std::move(hitbox) }, reactToKeyPress{ [](Model&, int) {} } {
			ModelLoader<Vertex>::loadModel(pathToModel, vertices, indexes);
			rotation = glm::quat(rotationEuler);
		}


		void setKeyPressResponse(std::function<void(Model&, int)> reactToKeyPress) {
			this->reactToKeyPress = reactToKeyPress;
		}


		const std::vector<Vertex>& getVertices() const {
			return vertices;
		}


		const std::vector<uint32_t>& getIndexes() const {
			return indexes;
		}


		Model& rotateModel(float angle, glm::vec3 axis) {
			rotation = glm::rotate(rotation, angle, axis);
			return *this;
		}


		const glm::mat4& calculateModelMatrix() const {
			return glm::translate(glm::mat4{ 1.0f }, glm::vec3(hitbox->getPosition())) *
				glm::scale(glm::mat4{ 1.0f }, glm::vec3{ hitbox->getScaleFactor() }) *
				glm::mat4{ hitbox->getRotation() * rotation};
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
		

		Vulkan::Physics::Hitbox& operator+() {
			return *hitbox;
		}
		

		void notifyKeyPress(int keyPressed) override {
			reactToKeyPress(*this, keyPressed);
		}


	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indexes;
		std::tuple<Matrices, Structs...> uniforms;

		glm::quat rotation; //in this simplified version of the physics (2D) a model can have a different rotation than the one of its hitbox

		std::unique_ptr<Vulkan::Physics::Hitbox> hitbox;

		std::function<void(Model&, int)> reactToKeyPress;
	};
}

#endif
