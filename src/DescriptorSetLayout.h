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
 * @brief Creates an object which describes the bindings of a set.
 */
class Vulkan::DescriptorSetLayout {
public:

	template<template<typename, typename> class... T, typename Type, typename Stage> requires (std::same_as<T<Type, Stage>, std::tuple<VkDescriptorType, VkShaderStageFlags>> && ...)
		DescriptorSetLayout(const LogicalDevice& virtualGpu, T<Type, Stage>... bindingsInfo) : virtualGpu{ virtualGpu } {
		//varaargs in a vector
		std::vector<T<Type, Stage>> bindingsInfoVector;
		(bindingsInfoVector.push_back(bindingsInfo), ...);

		std::vector<VkDescriptorSetLayoutBinding> bindings; //vector containing the bindings
		
		//create the descriptor for each binding in this set
		for (int i = 0; i < bindingsInfoVector.size(); ++i) {
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = i;
			binding.descriptorCount = 1; //no arrays for now
			binding.descriptorType = std::get<0>(bindingsInfoVector[i]);
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = std::get<1>(bindingsInfoVector[i]);
			bindings.push_back(binding);
		}

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

private:

	VkDescriptorSetLayout descriptorSetLayout;
	const LogicalDevice& virtualGpu;

};

#endif
