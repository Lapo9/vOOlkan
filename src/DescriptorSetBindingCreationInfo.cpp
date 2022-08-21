#include<vulkan/vulkan.h>

#include "DescriptorSetBindingCreationInfo.h"
#include "UniformBuffer.h"
#include "TextureImage.h"


Vulkan::DescriptorSetBindingCreationInfo::DescriptorSetBindingCreationInfo(unsigned int binding, const VkDescriptorSet& descriptorSet, int size, const Buffers::UniformBuffer& buffer, int offset) {
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
	descriptorInfo.pImageInfo = nullptr;
	descriptorInfo.pNext = nullptr;
}


Vulkan::DescriptorSetBindingCreationInfo::DescriptorSetBindingCreationInfo(unsigned int binding, const VkDescriptorSet& descriptorSet, const TextureImage& texture) {
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = +texture["base"];
	imageInfo.sampler = +texture.getSampler();

	descriptorInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorInfo.dstSet = descriptorSet;
	descriptorInfo.dstBinding = binding;
	descriptorInfo.dstArrayElement = 0;
	descriptorInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorInfo.descriptorCount = 1;
	descriptorInfo.pBufferInfo = nullptr;
	descriptorInfo.pImageInfo = &imageInfo;
	descriptorInfo.pNext = nullptr;
}



Vulkan::DescriptorSetBindingCreationInfo::DescriptorSetBindingCreationInfo(DescriptorSetBindingCreationInfo&& movedFrom) noexcept {
	descriptorInfo = movedFrom.descriptorInfo;
	bufferInfo = movedFrom.bufferInfo;
	imageInfo = movedFrom.imageInfo;

	if (descriptorInfo.pBufferInfo != nullptr) {
		descriptorInfo.pBufferInfo = &bufferInfo;
	}
	if (descriptorInfo.pImageInfo != nullptr) {
		descriptorInfo.pImageInfo = &imageInfo;
	}
}



const VkWriteDescriptorSet& Vulkan::DescriptorSetBindingCreationInfo::operator+() const {
	return descriptorInfo;
}