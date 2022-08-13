#ifndef VULKAN_DESCRIPTORSET
#define VULKAN_DESCRIPTORSET

#include <vulkan/vulkan.h>

#include "DecriptorSetPool.h"
#include "DescriptorSetLayout.h"
#include "LogicalDevice.h"
#include "VulkanException.h"


namespace Vulkan { class DescriptorSet; }

class Vulkan::DescriptorSet {
public:

	//FROMHERE this ctor should also get a UniformBuffer where to bind each binding of the layout linearly. There should also be another ctor which lets the user choose how to bind the bindings (so it should take one buffer and offset for each binding in the layout)
	DescriptorSet(const LogicalDevice& virtualGpu, const DescriptorSetPool& descriptorPool, const DescriptorSetLayout& layout, const UniformBuffer& buffer) : descriptorSet{} {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = +descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &+layout;

		if (VkResult result = vkAllocateDescriptorSets(+virtualGpu, &allocInfo, &descriptorSet); result != VK_SUCCESS) {
			throw VulkanException("Failed to allocate descriptor sets!", result);
		}

		//fill the created descriptors
		//TODO this should check whether the current descriptor is a dynamic buffer, a static buffer or an image and act accordingly
		std::vector<VkWriteDescriptorSet> descriptorsInfo;
		for (int i = 0, offset = 0; i < layout.getAmountOfBindings(); ++i) {
			descriptorsInfo.emplace_back(fillDescriptorSet(i, buffer, offset, layout.getSize(i)));
			offset += layout.getSize(i); //FIXTHIS padding
		}
		vkUpdateDescriptorSets(+virtualGpu, descriptorsInfo.size(), descriptorsInfo.data(), 0, nullptr);
	}
	

	const VkDescriptorSet& operator+() const {
		descriptorSet;
	}



private:

	//TODO make a similar function for Images and non-dynamic uniform buffer
	VkWriteDescriptorSet fillDescriptorSet(unsigned int binding, const UniformBuffer& buffer, unsigned int offset, unsigned int size) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = +buffer;
		bufferInfo.offset = offset;
		bufferInfo.range = size;

		VkWriteDescriptorSet descriptorInfo{};
		descriptorInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.dstSet = descriptorSet;
		descriptorInfo.dstBinding = binding;
		descriptorInfo.dstArrayElement = 0;
		descriptorInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		descriptorInfo.descriptorCount = 1;
		descriptorInfo.pBufferInfo = &bufferInfo;

		return descriptorInfo;
	}


	VkDescriptorSet descriptorSet;

};

#endif
