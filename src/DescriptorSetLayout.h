#ifndef VULKAN_DESCRIPTORSETLAYOUT
#define VULKAN_DESCRIPTORSETLAYOUT

#include <vulkan/vulkan.h>
#include <concepts>
#include <tuple>
#include <vector>

#include "VulkanException.h"
#include "LogicalDevice.h"


namespace Vulkan { class DescriptorSetLayout; }


/**
 * @brief Describes the bindings of a set, such as their size and number.
 */
class Vulkan::DescriptorSetLayout {
public:

	/**
	 * @brief Stores the info about the layout and creates a VkDescriptorSetLayout object.
	 * 
	 * @param virtualGpu The LogicalDevice.
	 * @param ...bindingsInfo Info about each binding of the set: type of binding (uniform, image, dynamic, ...), stage where the binding is used (vertex, fragment, all, ...), size in bytes of the binding.
	 */
	template<template<typename, typename, typename> class... T> requires (std::same_as<T<int, int, int>, std::tuple<int, int, int>> && ...)
		DescriptorSetLayout(const LogicalDevice& virtualGpu, T<VkDescriptorType, VkShaderStageFlagBits, int>... bindingsInfo) : virtualGpu{ virtualGpu }, sizes{} {
		//put varaargs in a vector
		std::vector<std::tuple<VkDescriptorType, VkShaderStageFlagBits, int>> bindingsInfoVector;
		(bindingsInfoVector.push_back(bindingsInfo), ...);

		std::vector<VkDescriptorSetLayoutBinding> bindings; //vector containing the binding info structures for this set
		
		//create the info structure for each binding in this set
		for (int i = 0; i < bindingsInfoVector.size(); ++i) {
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = i;
			binding.descriptorCount = 1; //no arrays for now
			binding.descriptorType = std::get<0>(bindingsInfoVector[i]);
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = std::get<1>(bindingsInfoVector[i]);
			bindings.push_back(binding);

			//save the size of each binding (used when instantiating the DescriptorSet in the fututre)
			sizes.push_back(std::get<2>(bindingsInfoVector[i]));
		}

		//put toghether the info structs for each binding and create the final layout object
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindingsInfoVector.size();
		layoutInfo.pBindings = bindings.data();

		if (VkResult result = vkCreateDescriptorSetLayout(+virtualGpu, &layoutInfo, nullptr, &descriptorSetLayout); result != VK_SUCCESS) {
			throw VulkanException("Failed to create descriptor set layout!", result);
		}
	}

	DescriptorSetLayout(const DescriptorSetLayout&) = delete;
	DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
	DescriptorSetLayout(DescriptorSetLayout&&) = delete;
	DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

	~DescriptorSetLayout() {
		vkDestroyDescriptorSetLayout(+virtualGpu, descriptorSetLayout, nullptr);
	}


	const VkDescriptorSetLayout& operator+() const {
		return descriptorSetLayout;
	}


	/**
	 * @brief Returns an array of the sizes (in bytes) of each binding in the set.
	 * 
	 * @return An array of the sizes (in bytes) of each binding in the set.
	 */
	const std::vector<int>& getSizes() const {
		return sizes;
	}


	/**
	 * @brief Returns the size (in bytes) of the i-th binding.
	 */
	int getSize(unsigned int i) const {
		return sizes[i];
	}


	/**
	 * @brief Returns the number of bindings in this set.
	 * 
	 * @return The number of bindings in this set..
	 */
	int getAmountOfBindings() const {
		return sizes.size();
	}


private:
	VkDescriptorSetLayout descriptorSetLayout;
	const LogicalDevice& virtualGpu;

	std::vector<int> sizes;

};

#endif
