#ifndef VULKAN_VERTEXINPUT
#define VULKAN_VERTEXINPUT

#include <glm/glm.hpp>
#include <concepts>
#include <type_traits>
#include <tuple>
#include <map>
#include <array>

#include "VulkanException.h"


namespace Vulkan::PipelineOptions {

	/**
	 * @brief Returns true if 1 <= n <= 4, Type is a fundalental typeand Vec<n, Type, packing> is a glm::vec<...>.
	 * @details It is mandatory to have a function (and not a concept with its requires clause, because n, Type and packing must be automatically deducted by the compiler.
	 */
	template<template<int, typename, glm::qualifier> class Vec, int n, typename Type, glm::qualifier packing>
	constexpr bool isGlmVec(Vec<n, Type, packing>) {
		return n > 0 && n <= 4 &&
			std::is_fundamental<Type>::value &&
			std::same_as <Vec<n, Type, packing>, glm::vec<n, Type, packing>>;
	}


	/**
	 * @brief This class is used to obtain the basic properties of a vertex, used to create bindings for the Vulkan pipeline.
	 * @details This properties are:
	 * - stride: basically the bytes occupied by a single vertex. e.g. { {int, int, int}, {double, double} } has stride 3*4 + 2*8 = 28.
	 * - for each component of the vertex:
	 *	- scalar amount: the amount of "coordinates" that make up the component. e.g. { {x, y, z}, {u, v} } has one component with 3 and one with 2.
	 *	- scalar size: the size of one single coordinate. e.g. { {int, int, int}, {double, double} } has one component with size 32/8=4 and one with size 64/8=8.
	 *	- format: a VkFormat for the component which depends on the scalar amount and size. e.g. {float, float, float} has VK_FORMAT_R32G32B32_SFLOAT.
	 *	- offset: the offset in bytes of the first element of the component w.r.t. the starting position of its Vertex. e.g. { {int, int, int}, {double, double} } has offsets 0 and 3*4=12.
	 */
	template<int componentsAmount>
	class VertexProperties {
	private:

		/**
		 * @brief Properties of a single component. Look at VertexProperties description.
		 */
		struct ComponentProperties {
			unsigned int scalarAmount;
			size_t scalarSize;
			size_t offset;
			VkFormat format;
		};

		std::array<ComponentProperties, componentsAmount> componentsProperties;
		size_t stride;
		int sizeOfVertex;
		int arrayDimension = 0; //how many "complete" elements are in the array


		/**
		 * @brief Extracts the properties of a component and adds them to the array.
		 * @details This function should only be called by the ctor, indeed this function keeps track of how many time it has been called, so that she knows where to add the element into the array.
		 *			This function is able to extract the properties of a component thanks to template automatic argument deduction.
		 *			This function assumes that the actual values of the vertex components are saved inside the Vertex class in the same order as this function receives them, in contiguous positions, without any member variable preceeding them.
		 *
		 * @param The component to extract the properties from.
		 * @tparam Vec the glm::vec<n, Typem packing> which is the component.
		 * @tparam n Number of coordinates of the component.
		 * @tparam Type Type of each coordinate of the component (generally int, float, double).
		 * @tparam packing
		 */
		template<template<int, typename, glm::qualifier> class Vec, int n, typename Type, glm::qualifier packing> requires(isGlmVec(Vec<n, Type, packing>{}))
			void addComponent(Vec<n, Type, packing>) {
			componentsProperties.at(arrayDimension) = ComponentProperties{ n, sizeof(Type), stride }; //the offset of this component is equivalent to the current stride
			componentsProperties.at(arrayDimension).format = findRightFormat<Type, n>(); //calculate the VkFormat (no arguments are passed because it is only important the Type and coordinates amount to calculate the format)
			stride += sizeof(Type) * n; //increase the stride, namely the dimension of this Vertex
			arrayDimension++;
		}


		/**
		 * @brief Given the Type and the amount of coordinates of a component, it returns the right VkFormat.
		 *
		 * @return The right VkFormat for the specifies type of component.
		 */
		template<typename Type, int scalarAmount>
		static VkFormat findRightFormat() {
			//build the string which match the enum value name, then lookup a map containing <string, enum>
			std::string format = "VK_FORMAT_";
			std::string length = std::to_string(sizeof(Type) * 8); //length of the underying type, in bits

			//add the RxxGxxBxxAxx part
			for (int i = 1; i <= scalarAmount; ++i) {
				format += RGBA(i) + length;
			}
			format += "_";

			std::string type = std::is_floating_point_v<Type> ? "SFLOAT" :
				std::is_unsigned_v<Type> ? "UINT" : "SINT";
			format += type;

			//from the string now get the enum value
			std::map<std::string, VkFormat> formats = {
				{"VK_FORMAT_R16_UINTx",					VK_FORMAT_R16_UINT},
				{"VK_FORMAT_R16_SINT",				VK_FORMAT_R16_SINT},
				{"VK_FORMAT_R16_SFLOAT",			VK_FORMAT_R16_SFLOAT},
				{"VK_FORMAT_R16G16_UNORM",			VK_FORMAT_R16G16_UNORM},
				{"VK_FORMAT_R16G16_SNORM",			VK_FORMAT_R16G16_SNORM},
				{"VK_FORMAT_R16G16_USCALED",		VK_FORMAT_R16G16_USCALED},
				{"VK_FORMAT_R16G16_SSCALED",		VK_FORMAT_R16G16_SSCALED},
				{"VK_FORMAT_R16G16_UINT",			VK_FORMAT_R16G16_UINT},
				{"VK_FORMAT_R16G16_SINT",			VK_FORMAT_R16G16_SINT},
				{"VK_FORMAT_R16G16_SFLOAT",			VK_FORMAT_R16G16_SFLOAT},
				{"VK_FORMAT_R16G16B16_UNORM",		VK_FORMAT_R16G16B16_UNORM},
				{"VK_FORMAT_R16G16B16_SNORM",		VK_FORMAT_R16G16B16_SNORM},
				{"VK_FORMAT_R16G16B16_USCALED",		VK_FORMAT_R16G16B16_USCALED},
				{"VK_FORMAT_R16G16B16_SSCALED",		VK_FORMAT_R16G16B16_SSCALED},
				{"VK_FORMAT_R16G16B16_UINT",		VK_FORMAT_R16G16B16_UINT},
				{"VK_FORMAT_R16G16B16_SINT",		VK_FORMAT_R16G16B16_SINT},
				{"VK_FORMAT_R16G16B16_SFLOAT",		VK_FORMAT_R16G16B16_SFLOAT},
				{"VK_FORMAT_R16G16B16A16_UNORM",	VK_FORMAT_R16G16B16A16_UNORM},
				{"VK_FORMAT_R16G16B16A16_SNORM",	VK_FORMAT_R16G16B16A16_SNORM},
				{"VK_FORMAT_R16G16B16A16_USCALED",	VK_FORMAT_R16G16B16A16_USCALED},
				{"VK_FORMAT_R16G16B16A16_SSCALED",	VK_FORMAT_R16G16B16A16_SSCALED},
				{"VK_FORMAT_R16G16B16A16_UINT",		VK_FORMAT_R16G16B16A16_UINT},
				{"VK_FORMAT_R16G16B16A16_SINT",		VK_FORMAT_R16G16B16A16_SINT},
				{"VK_FORMAT_R16G16B16A16_SFLOAT",	VK_FORMAT_R16G16B16A16_SFLOAT},
				{"VK_FORMAT_R32_UINT",				VK_FORMAT_R32_UINT},
				{"VK_FORMAT_R32_SINT",				VK_FORMAT_R32_SINT},
				{"VK_FORMAT_R32_SFLOAT",			VK_FORMAT_R32_SFLOAT},
				{"VK_FORMAT_R32G32_UINT",			VK_FORMAT_R32G32_UINT},
				{"VK_FORMAT_R32G32_SINT",			VK_FORMAT_R32G32_SINT},
				{"VK_FORMAT_R32G32_SFLOAT",			VK_FORMAT_R32G32_SFLOAT},
				{"VK_FORMAT_R32G32B32_UINT",		VK_FORMAT_R32G32B32_UINT},
				{"VK_FORMAT_R32G32B32_SINT",		VK_FORMAT_R32G32B32_SINT},
				{"VK_FORMAT_R32G32B32_SFLOAT",		VK_FORMAT_R32G32B32_SFLOAT},
				{"VK_FORMAT_R32G32B32A32_UINT",		VK_FORMAT_R32G32B32A32_UINT},
				{"VK_FORMAT_R32G32B32A32_SINT",		VK_FORMAT_R32G32B32A32_SINT},
				{"VK_FORMAT_R32G32B32A32_SFLOAT",	VK_FORMAT_R32G32B32A32_SFLOAT},
				{"VK_FORMAT_R64_UINT",				VK_FORMAT_R64_UINT},
				{"VK_FORMAT_R64_SINT",				VK_FORMAT_R64_SINT},
				{"VK_FORMAT_R64_SFLOAT",			VK_FORMAT_R64_SFLOAT},
				{"VK_FORMAT_R64G64_UINT",			VK_FORMAT_R64G64_UINT},
				{"VK_FORMAT_R64G64_SINT",			VK_FORMAT_R64G64_SINT},
				{"VK_FORMAT_R64G64_SFLOAT",			VK_FORMAT_R64G64_SFLOAT},
				{"VK_FORMAT_R64G64B64_UINT",		VK_FORMAT_R64G64B64_UINT},
				{"VK_FORMAT_R64G64B64_SINT",		VK_FORMAT_R64G64B64_SINT},
				{"VK_FORMAT_R64G64B64_SFLOAT",		VK_FORMAT_R64G64B64_SFLOAT},
				{"VK_FORMAT_R64G64B64A64_UINT",		VK_FORMAT_R64G64B64A64_UINT},
				{"VK_FORMAT_R64G64B64A64_SINT",		VK_FORMAT_R64G64B64A64_SINT},
				{"VK_FORMAT_R64G64B64A64_SFLOAT", VK_FORMAT_R64G64B64A64_SFLOAT}
			};

			return formats[format];
		}


		/**
		 * @brief 1=R, 2=G, 3=B, 4=A.
		 */
		static std::string RGBA(int i) {
			switch (i) {
			case 1:
				return "R";
				break;
			case 2:
				return "G";
				break;
			case 3:
				return "B";
				break;
			case 4:
				return "A";
				break;
			default:
				throw VulkanException("Error in the amount of scala amount in the component of a Vertex: 1 <= n <= 4, but n == " + i);
				break;
			}
		}


	public:
		/**
		 * @brief Extract all of the properties from this Vertex.
		 * @details Check that each component is indeed a GlmVec via the requires clause.
		 *
		 * @param sizeOfVertex The size of the Vertex from where this function has been called. It is important to know the size of the Vertex in order to calculate the stride.
		 * @param ...components Each component of this vertex.
		 */
		template<typename... Vec> requires (isGlmVec(Vec{}) && ...)
			VertexProperties(int sizeOfVertex = -1, Vec... components) : stride{ 0 }, sizeOfVertex{ sizeOfVertex }, componentsProperties{} {
			(addComponent(components), ...); //for each component, extract its properties
			stride = sizeOfVertex; //the stride should already be set by the addComponent loop in the instruction above, but if the Vertex type (Vec) has some other data in addition to the components, the stride should be different than the one calculated
		}


		/**
		 * @brief Returns the properties of the specified component.
		 *
		 * @param i The component index to return the properties.
		 * @return The properties of the specified component.
		 */
		const ComponentProperties& operator[](unsigned int i) {
			return componentsProperties.at(i);
		}


		/**
		 * @brief Returns whether the object has been initialized or not.
		 * @details If the object has been default initialized, then sizeOfVertex is negative.
		 * It is so because this class should only be used inside the Vertex class as a static member, so, when it is first initialized, it is impossible (or, at least, unconvenient) to know the Vertex size.
		 *
		 * @return Whether the object has been initialized or not.
		 */
		bool isInitialized() const {
			return sizeOfVertex > 0;
		}
	};




	/**
	 * @brief This class represents a vertex, which is made up of many components, such as space coordinates, texture coordinates, color, normals, ...
	 * @details This class should be used to define the type of the vertex via a using directive, e.g.:
	 * @code using MyVertexType = Vertex<glm::vec3, glm::vec2>; MyVertexType v1 = {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};
	 * @tparam Vec... Each Vec is a component of the vertex. A Vec must be a glm::vec<n, Type, packing>, and this is checked at compile time thanks to automatic template argument deduction.
	 */
	template<typename... Vec> requires (isGlmVec(Vec{}) && ...)
		class Vertex {
		public:
			/**
			 * @brief Creates a new vertex.
			 *
			 * @param ...vertexComponents All of the components of this vertex.
			 */
			Vertex(Vec... vertexComponents) : vertexComponents{ vertexComponents... } {
				if (!vertexProperties.isInitialized()) {
					vertexProperties = VertexProperties<sizeof...(Vec)>{ sizeof(this), vertexComponents... };
				}
			}


			Vertex() : vertexComponents{} {}


			/**
			 * @brief Returns the descriptors for this type of vertex, namely VkVertexInputBindingDescription and VkVertexInputAttributeDescription.
			 *
			 * @param binding Vulkan pipeline binding location.
			 * @return The Vertex descriptor with the specified binding, and all of the descriptors for each component of the Vertex.
			 */
			static std::pair<VkVertexInputBindingDescription, std::vector<VkVertexInputAttributeDescription>> getDescriptors(unsigned int binding) {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = binding;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				std::vector <VkVertexInputAttributeDescription> attributeDescriptions(sizeof...(Vec)); //the array must have as many elements as the number of components
				for (int i = 0; i < sizeof...(Vec); ++i) {
					attributeDescriptions[i].binding = binding;
					attributeDescriptions[i].location = i;
					attributeDescriptions[i].format = vertexProperties[i].format;
					attributeDescriptions[i].offset = vertexProperties[i].offset;
				}

				return { bindingDescription, attributeDescriptions };
			}


		private:
			std::tuple<Vec...> vertexComponents;
			inline static VertexProperties<sizeof...(Vec)> vertexProperties;
	};



	template<typename T>
	concept IsVertex = requires(T, unsigned int i) {
		{T::getDescriptors(i)} -> std::same_as<std::pair<VkVertexInputBindingDescription, std::vector<VkVertexInputAttributeDescription>>>;
	};


	/**
	 * @brief This class holds all of the vertices format infos about the vertices types which are used in a pipeline.
	 * 
	 */
	class PipelineVertexArrays {
	public:

		template<IsVertex... V>
		PipelineVertexArrays(V...) : vertexArraysDescriptor{} {
			{
				int i = 0; //counter of how many vertex types has already been added
				auto extract = [this, &i]<IsVertex T>()->void {
					auto [bindingDescr, attributeDescrs] = T::getDescriptors(i); //get the descriptors for this vertex type
					//add them to the arrays
					this->bindingDescriptors.push_back(bindingDescr);
					this->attributeDescriptors.insert(this->attributeDescriptors.end(), attributeDescrs.begin(), attributeDescrs.end());
					i++;
				};
				((extract.template operator() < V > ()), ...); //calls the lambda for each Vertex type that has been passed as argument
			}

			//fill the descriptor
			vertexArraysDescriptor.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexArraysDescriptor.vertexBindingDescriptionCount = bindingDescriptors.size();
			vertexArraysDescriptor.pVertexBindingDescriptions = bindingDescriptors.data();
			vertexArraysDescriptor.vertexAttributeDescriptionCount = attributeDescriptors.size();
			vertexArraysDescriptor.pVertexAttributeDescriptions = attributeDescriptors.data();
		}


		/**
		 * @brief Returns the underlying VkPipelineVertexInputStateCreateInfo object.
		 * 
		 * @return The underlying VkPipelineVertexInputStateCreateInfo object.
		 */
		const VkPipelineVertexInputStateCreateInfo& operator+() const {
			return vertexArraysDescriptor;
		}

	private:
		VkPipelineVertexInputStateCreateInfo vertexArraysDescriptor;
		std::vector<VkVertexInputBindingDescription> bindingDescriptors;
		std::vector< VkVertexInputAttributeDescription> attributeDescriptors;
	};

}


#endif
