#ifndef VULKAN_DESCRIPTORSET
#define VULKAN_DESCRIPTORSET

#include <vulkan/vulkan.h>

#include "DescriptorSetPool.h"
#include "Set.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "UniformBuffer.h"
#include "TextureImage.h"
#include "VulkanException.h"


namespace Vulkan { class DescriptorSet; }


/**
 * @brief A DescriptorSet is an object which holds the handles (pointers) to the bindings (variables) in a specific set.
 * @details If the layout of the set is { Data 40bytes, Data 6bytes, Data 10bytes }, then, by default, the descriptor set will hold 3 pointers into a UniformBuffer.
 */
class Vulkan::DescriptorSet {
public:

	/**
	 * @brief This ctor sets the pointers into the buffer in a linear way, by grouping the resources in a per-object fashion. Padding is considered.
	 * @details e.g. data1 -> 40bytes, data2 -> 6bytes, data3 -> 10bytes, alignment = 16bytes.
	 *			data1 starting position = 0, data2 starting position = 48, data3 starting position = 64.
	 *			Of course the data in the buffer must respect this layout.
	 * 
	 * @param virtualGpu The LogicalDevice.
	 * @param realGpu The PhysicalDevice.
	 * @param descriptorPool The pool from which allocate the buffers.
	 * @param set The set from which to create this descriptor set. A Set has all the info about the layout of the bindings, such as their size and the buffers where they must be stored.
	 */
	DescriptorSet(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, const DescriptorSetPool& descriptorPool, const Set& set) : set{ set } {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = +descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &+set;

		if (VkResult result = vkAllocateDescriptorSets(+virtualGpu, &allocInfo, &descriptorSet); result != VK_SUCCESS) {
			throw VulkanException("Failed to allocate descriptor sets!", result);
		}

		//fill the created descriptors
		//TODO this should check whether the current descriptor is a dynamic buffer, a static buffer or an image and act accordingly
		std::vector<VkWriteDescriptorSet> descriptorsInfo;
		std::vector<VkDescriptorBufferInfo> descriptorsBufferInfo(set.getAmountOfBindings()); //this is necessary in order to have a reference to the buffer info struct used by the descriptors info. If we created the object inside the fill function, we would lose the reference as soon as the function returns.
		for (int i = 0; i < set.getAmountOfBindings(); ++i) {
			descriptorsInfo.emplace_back(fillDescriptorSet(i, set.getBindingInfo(i).buffer, set.getBindingInfo(i).offset, set.getBindingInfo(i).size, descriptorsBufferInfo[i]));
		}
		
		vkUpdateDescriptorSets(+virtualGpu, descriptorsInfo.size(), descriptorsInfo.data(), 0, nullptr);
	}
	

	const VkDescriptorSet& operator+() const {
		return descriptorSet;
	}


	/**
	* @brief Returns the offsets of each binding from the first binding of each type (useful for dynamic buffers which hold a binding for each object).
	* @details e.g. If we have the buffer structured as AAAABB AAAABB AAAABB, then the offsets will be 6 for A binding and 6 for B binding.
	*			If we had AAAA AAAA AAAA BB BB BB then the offsets would be 4 for A binding and 2 for B binding.
	* @param multiplier The multiplier is used to get the offset of a specific binding when the buffer is allocated dynamically (so it holds a binding for each object), e.g. in the second example with multiplier = 7 we would get {28, 14}
	* @return The offsets.
	*/
	std::vector<uint32_t> getOffsets(unsigned int multiplier = 0) const {
		auto bindingsInfo = set.getBindingsInfo();
		int totalSize = 0; //dimension of the sum of all of the bindings of one object
		for (const auto& bindingInfo : bindingsInfo) {
			totalSize += bindingInfo.size;
		}
		return std::vector<uint32_t>(set.getAmountOfBindings(), totalSize * multiplier);
	}


	/**
	 * @brief Returns how many dynamic bindings are present in this descriptor set.
	 * 
	 * @return How many dynamic bindings are present in this descriptor set.
	 */
	int getAmountOfBindings() const {
		return set.getAmountOfBindings();
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


	VkWriteDescriptorSet fillDescriptorSet(unsigned int binding, const TextureImage& texture, VkDescriptorImageInfo imageInfo) {
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = +texture["base"];
		imageInfo.sampler = +texture.getSampler();

		VkWriteDescriptorSet descriptorInfo{};
		descriptorInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorInfo.dstSet = descriptorSet;
		descriptorInfo.dstBinding = binding;
		descriptorInfo.dstArrayElement = 0;
		descriptorInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorInfo.descriptorCount = 1;
		descriptorInfo.pImageInfo = &imageInfo;
	}


	VkDescriptorSet descriptorSet;
	const Set& set;
};

#endif
