#ifndef VULKAN_DYNAMICDESCRIPTORSET
#define VULKAN_DYNAMICDESCRIPTORSET

#include <vulkan/vulkan.h>

#include "DescriptorSetPool.h"
#include "DynamicSet.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "UniformBuffer.h"
#include "TextureImage.h"
#include "VulkanException.h"
#include "DescriptorSet.h"


namespace Vulkan { class DynamicDescriptorSet; }


/**
 * @brief A DynamicDescriptorSet is an object which holds the handles (pointers) to the bindings (variables) in a specific set.
 * @details If the layout of the set is { Data 40bytes, Data 6bytes, Data 10bytes }, then, by default, the descriptor set will hold 3 pointers into a UniformBuffer.
 */
class Vulkan::DynamicDescriptorSet : public DescriptorSet<Vulkan::DynamicSetBindingInfo, Vulkan::DynamicSet> {
public:

	/**
	 * @brief Builds a DynamicDescriptorSet by following the information of the passed-in set.
	 * 
	 * @param virtualGpu The LogicalDevice.
	 * @param realGpu The PhysicalDevice.
	 * @param descriptorPool The pool from which allocate the buffers.
	 * @param set The set from which to create this descriptor set. A DynamicSet has all the info about the layout of the bindings, such as their size and the buffers where they must be stored.
	 */
	DynamicDescriptorSet(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, const DescriptorSetPool& descriptorPool, const DynamicSet& set) : DescriptorSet{ virtualGpu, descriptorPool, set } {
		//fill the created descriptors
		//TODO this should check whether the current descriptor is a dynamic buffer, a static buffer or an image and act accordingly
		std::vector<VkWriteDescriptorSet> descriptorsInfo;
		std::vector<VkDescriptorBufferInfo> descriptorsBufferInfo(set.getAmountOfBindings()); //this is necessary in order to have a reference to the buffer info struct used by the descriptors info. If we created the object inside the fill function, we would lose the reference as soon as the function returns.
		for (int i = 0; i < set.getAmountOfBindings(); ++i) {
			descriptorsInfo.emplace_back(fillDescriptorSet(i, set.getBindingInfo(i).buffer, set.getBindingInfo(i).offset, set.getBindingInfo(i).size, descriptorsBufferInfo[i]));
		}
		
		vkUpdateDescriptorSets(+virtualGpu, descriptorsInfo.size(), descriptorsInfo.data(), 0, nullptr);
	}
	

	/**
	* @brief Returns the offsets of each binding from the first binding of each type (useful for dynamic buffers which hold a binding for each object).
	* @details e.g. If we have the buffer structured as AAAABB AAAABB AAAABB, then the offsets will be 6 for A binding and 6 for B binding.
	*			If we had AAAA AAAA AAAA BB BB BB then the offsets would be 4 for A binding and 2 for B binding.
	* @param multiplier The multiplier is used to get the offset of a specific binding when the buffer is allocated dynamically (so it holds a binding for each object), e.g. in the second example with multiplier = 7 we would get {28, 14}
	* @return The offsets.
	*/
	std::vector<uint32_t> getOffsets(unsigned int multiplier = 0) const {
		std::vector<uint32_t> res;
		for (const auto& bindingInfo : set.getBindingsInfo()) {
			res.push_back(bindingInfo.dynamicDistance * multiplier);
		}
		return res;
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
};

#endif
