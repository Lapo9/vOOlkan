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

	template<template<typename, typename, typename> class... T> requires (std::same_as<T<int, int, int>, std::tuple<int, int, int>> && ...)
		DescriptorSetLayout(const LogicalDevice& virtualGpu, T<VkDescriptorType, VkShaderStageFlags, int>... bindingsInfo) : virtualGpu{ virtualGpu }, sizes{} {
		//varaargs in a vector
		std::vector<T<VkDescriptorType, VkShaderStageFlags, int>> bindingsInfoVector;
		(bindingsInfoVector.push_back(bindingsInfo), ...);

		std::vector<VkDescriptorSetLayoutBinding> bindings; //vector containing the bindings for this set
		
		//create the descriptor for each binding in this set
		for (int i = 0; i < bindingsInfoVector.size(); ++i) {
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = i;
			binding.descriptorCount = 1; //no arrays for now
			binding.descriptorType = std::get<0>(bindingsInfoVector[i]);
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = std::get<1>(bindingsInfoVector[i]);
			bindings.push_back(binding);

			//save the size of each binding
			sizes.push_back(std::get<2>(bindingsInfoVector[i]));
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


	const VkDescriptorSetLayout& operator+() const {
		return descriptorSetLayout;
	}


	const std::vector<int>& getSizes() {
		return sizes;
	}

	int getSize(unsigned int i) {
		return sizes[i];
	}


private:
	VkDescriptorSetLayout descriptorSetLayout;
	const LogicalDevice& virtualGpu;

	std::vector<int> sizes;

};

#endif
