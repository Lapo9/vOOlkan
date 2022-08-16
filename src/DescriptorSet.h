#ifndef VULKAN_DESCRIPTORSET
#define VULKAN_DESCRIPTORSET

#include <vulkan/vulkan.h>

#include "DescriptorSetPool.h"
#include "DescriptorSetLayout.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "UniformBuffer.h"
#include "VulkanException.h"


namespace Vulkan { class DescriptorSet; }

class Vulkan::DescriptorSet {
public:

	//TODO there should also be another ctor which lets the user choose how to bind the bindings (so it should take one buffer and offset for each binding in the layout)
	DescriptorSet(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, const DescriptorSetPool& descriptorPool, const DescriptorSetLayout& layout, const Buffers::UniformBuffer& buffer) : descriptorSet{} {
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
		std::vector<VkDescriptorBufferInfo> descriptorsBufferInfo(layout.getAmountOfBindings()); //this is necessary in order to have a reference to the buffer info struct used by the descriptors info. If we created the object inside the fill function, we would lose the reference as soon as the function returns
		int offset = 0;

		//get minimum alignment for the GPU memory, used for padding
		VkPhysicalDeviceProperties limits;
		vkGetPhysicalDeviceProperties(+realGpu, &limits);
		int alignment = limits.limits.minUniformBufferOffsetAlignment;

		for (int i = 0; i < layout.getAmountOfBindings(); ++i) {
			descriptorsInfo.emplace_back(fillDescriptorSet(i, buffer, offset, layout.getSize(i), descriptorsBufferInfo[i]));
			offset += layout.getSize(i);
			int paddingAmount = (alignment - (offset % alignment)) % alignment; //number of padding bytes
			offset += paddingAmount; //padding
		}
		offsets.insert(offsets.begin(), layout.getAmountOfBindings(), offset); //we allocate the vector like AAAABB AAAABB AAAABB, so the offsets between adjacent bindings are always the same

		vkUpdateDescriptorSets(+virtualGpu, descriptorsInfo.size(), descriptorsInfo.data(), 0, nullptr);
	}
	

	const VkDescriptorSet& operator+() const {
		return descriptorSet;
	}


	/**
	* @brief Returns the offsets of each binding from the first binding of each type.
	* @details e.g. we have the buffer structured as AAAABB AAAABB AAAABB, then the offsets will be 6 for A binding and 6 for B binding.
	*			If we had AAAA AAAA AAAA BB BB BB then the offsets would be 4 for A binding and 2 for B binding.
	*			The multiplier is used to get the offset of a specific binding, e.g. in the second example with multiplier = 7 we would get {28, 14}
	*/
	std::vector<uint32_t> getOffsets(unsigned int multiplier = 0) const {
		std::vector<uint32_t> res;
		for (const auto& offset : offsets) {
			res.push_back(offset * multiplier);
		}
		return res;
	}


	int getAmountOfOffsets() const {
		return offsets.size();
	}


private:

	//TODO make a similar function for Images and non-dynamic uniform buffer
	VkWriteDescriptorSet fillDescriptorSet(unsigned int binding, const Buffers::UniformBuffer& buffer, unsigned int offset, unsigned int size, VkDescriptorBufferInfo& bufferInfo) {
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
	std::vector<int> offsets; //distance between 2 consecutive bindings. e.g. AAAABB AAAABB AAAABB offsets = {6,6}. AAAA AAAA AAAA BB BB BB offsets = {4, 2} (in the examples padding is not considered)

};

#endif
