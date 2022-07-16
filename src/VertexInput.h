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

	template<template<int, typename, glm::qualifier> class Vec, int n, typename Type, glm::qualifier packing>
	constexpr bool isGlmVec(Vec<n, Type, packing>) {
		return n > 0 && n <= 4 &&
			std::is_fundamental<Type>::value &&
			std::same_as <Vec<n, Type, packing>, glm::vec<n, Type, packing>>;
	}



	template<int componentsAmount>
	class VertexProperties {
	private:
		struct ComponentProperties {
			unsigned int scalarAmount;
			size_t scalarSize;
			size_t offset;
			VkFormat format;
		};

		std::array<ComponentProperties, componentsAmount> componentsProperties;
		size_t stride;
		int arrayDimension; //how many "complete" elements are in the array


		template<template<int, typename, glm::qualifier> class Vec, int n, typename Type, glm::qualifier packing>
		constexpr void addComponent(Vec<n, Type, packing>) {
			componentsProperties.at(arrayDimension) = ComponentProperties{ n, sizeof(Type), stride };
			componentsProperties.at(arrayDimension).format = findRightFormat<Type, n>();
			stride += sizeof(Type) * n;
			arrayDimension++;
		}


		template<typename Type, int scalarAmount>
		static VkFormat findRightFormat() {
			std::string format = "VK_FORMAT_";
			std::string length = std::to_string(sizeof(Type) * 8);

			for (int i = 1; i <= scalarAmount; ++i) {
				format += length + RGBA(i);
			}
			format += "_";

			std::string type = std::is_floating_point_v<Type> ? "SFLOAT" :
				std::is_unsigned_v<Type> ? "UINT" : "SINT";
			format += type;


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

		template<typename... Vec> requires (isGlmVec(Vec{}) && ...)
			constexpr VertexProperties(Vec... components) : stride{ 0 }, arrayDimension{ 0 } {
			(addComponent(components), ...);
		}


		const std::array<ComponentProperties, componentsAmount>& operator[](unsigned int i) {
			return componentsProperties.at[i];
		}
	};


	

	template<typename... Vec> requires (isGlmVec(Vec{}) && ...)
	class Vertex {
	public:
		Vertex(Vec... vertexComponents) : vertexComponents{ vertexComponents... }, vertexProperties{ vertexComponents... } {
			
		}


		static std::pair<VkVertexInputBindingDescription, std::array<VkVertexInputAttributeDescription, sizeof...(Vec)>> getDescriptors(unsigned int binding) {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = binding;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			std::array<VkVertexInputAttributeDescription, sizeof...(Vec)> attributeDescriptions;
			for (int i = 0; i < sizeof...(Vec); ++i) {
				attributeDescriptions[i].binding = binding;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[i].offset = vertexProperties[i].offset;
			}
		}

	private:
		std::tuple<Vec...> vertexComponents;
		VertexProperties<sizeof...(Vec)> vertexProperties;
	};
}

#endif
