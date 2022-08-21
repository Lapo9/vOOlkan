#ifndef VULKAN_DESCRIPTIONSETBINDINGCREATIONINFO
#define VULKAN_DESCRIPTIONSETBINDINGCREATIONINFO

#include <vulkan/vulkan.h>


namespace Vulkan { class DescriptorSetBindingCreationInfo; class TextureImage; namespace Buffers { class UniformBuffer; } }


class Vulkan::DescriptorSetBindingCreationInfo {
public:
	DescriptorSetBindingCreationInfo(unsigned int binding, const VkDescriptorSet& descriptorSet, int size, const Buffers::UniformBuffer& buffer, int offset, int dynamicDistance);
	
	
	DescriptorSetBindingCreationInfo(unsigned int binding, const VkDescriptorSet& descriptorSet, int size, const Buffers::UniformBuffer& buffer, int offset);


	DescriptorSetBindingCreationInfo(unsigned int binding, const VkDescriptorSet& descriptorSet, const TextureImage& texture);


	DescriptorSetBindingCreationInfo(const DescriptorSetBindingCreationInfo&) = delete;
	DescriptorSetBindingCreationInfo& operator=(const DescriptorSetBindingCreationInfo&) = delete;
	DescriptorSetBindingCreationInfo& operator=(DescriptorSetBindingCreationInfo&&) = delete;

	DescriptorSetBindingCreationInfo(DescriptorSetBindingCreationInfo&& movedFrom) noexcept;



	const VkWriteDescriptorSet& operator+() const;


private:
	VkWriteDescriptorSet descriptorInfo;
	VkDescriptorBufferInfo bufferInfo;
	VkDescriptorImageInfo imageInfo;
};


#endif
