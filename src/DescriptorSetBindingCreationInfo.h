#ifndef VULKAN_DESCRIPTIONSETBINDINGCREATIONINFO
#define VULKAN_DESCRIPTIONSETBINDINGCREATIONINFO

#include <vulkan/vulkan.h>

#include "UniformBuffer.h"


namespace Vulkan { class DescriptorSetBindingCreationInfo; }


class Vulkan::DescriptorSetBindingCreationInfo {
public:
	DescriptorSetBindingCreationInfo(unsigned int binding, const VkDescriptorSet& descriptorSet, int size, const Buffers::UniformBuffer& buffer, int offset) : descriptorInfo{}, bufferInfo{} {
		bufferInfo.buffer = +buffer;
		bufferInfo.offset = offset;
		bufferInfo.range = size;

		descriptorInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.dstSet = descriptorSet;
		descriptorInfo.dstBinding = binding;
		descriptorInfo.dstArrayElement = 0;
		descriptorInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		descriptorInfo.descriptorCount = 1;
		descriptorInfo.pBufferInfo = &bufferInfo;
		descriptorInfo.pNext = nullptr;
	}


	DescriptorSetBindingCreationInfo(const DescriptorSetBindingCreationInfo&) = delete;
	DescriptorSetBindingCreationInfo& operator=(const DescriptorSetBindingCreationInfo&) = delete;
	DescriptorSetBindingCreationInfo& operator=(DescriptorSetBindingCreationInfo&&) = delete;

	DescriptorSetBindingCreationInfo(DescriptorSetBindingCreationInfo&& movedFrom) noexcept {
		descriptorInfo = movedFrom.descriptorInfo;
		bufferInfo = movedFrom.bufferInfo;

		descriptorInfo.pBufferInfo = &bufferInfo;
	}



	const VkWriteDescriptorSet& operator+() const {
		return descriptorInfo;
	}


//private:
	VkWriteDescriptorSet descriptorInfo;
	VkDescriptorBufferInfo bufferInfo;
};


#endif
