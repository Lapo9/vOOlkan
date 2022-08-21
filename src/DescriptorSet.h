#ifndef VULKAN_DESCRIPTORSET
#define VULKAN_DESCRIPTORSET

#include <vulkan/vulkan.h>
#include <concepts>

#include "DescriptorSetPool.h"
#include "Set.h"
#include "DynamicSet.h"
#include "StaticSet.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "UniformBuffer.h"
#include "TextureImage.h"
#include "VulkanException.h"


namespace Vulkan {


	/**
	 * @brief A DescriptorSet is an object which holds the handles (pointers) to the bindings (variables) in a specific set.
	 * @details If the layout of the set is { Data 40bytes, Data 6bytes, Data 10bytes }, then, by default, the descriptor set will hold 3 pointers into a UniformBuffer.
	 */
	template<std::derived_from<Vulkan::Set> S>
	class DescriptorSet {
	public:

		/**
		 * @brief Builds a DescriptorSet by following the information of the passed-in set.
		 *
		 * @param virtualGpu The LogicalDevice.
		 * @param realGpu The PhysicalDevice.
		 * @param descriptorPool The pool from which allocate the buffers.
		 * @param set The set from which to create this descriptor set. A DynamicSet has all the info about the layout of the bindings, such as their size and the buffers where they must be stored.
		 */
		DescriptorSet(const LogicalDevice& virtualGpu, const DescriptorSetPool& descriptorPool, const S& set) : set{ set } {
			//create the descriptor set
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = +descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &+set;

			if (VkResult result = vkAllocateDescriptorSets(+virtualGpu, &allocInfo, &descriptorSet); result != VK_SUCCESS) {
				throw VulkanException("Failed to allocate descriptor sets!", result);
			}


			//fill the descriptor set's bindings
			std::vector<DescriptorSetBindingCreationInfo> descriptorsInfo;
			for (int i = 0; i < set.getAmountOfBindings(); ++i) {
				descriptorsInfo.push_back(set.getBindingInfo(i).generateDescriptorSetBindingInfo(i, descriptorSet));
			}

			std::vector<VkWriteDescriptorSet> rawDescriptorsInfo;
			for (const auto& descriptorInfo : descriptorsInfo) {
				rawDescriptorsInfo.push_back(+descriptorInfo);
			}

			vkUpdateDescriptorSets(+virtualGpu, rawDescriptorsInfo.size(), rawDescriptorsInfo.data(), 0, nullptr);
		}


		const VkDescriptorSet& operator+() const {
			return descriptorSet;
		}


		/**
		 * @brief Returns the underlying set of this descriptor set.
		 */
		const S& getSet() const {
			return set;
		}


	private:
		VkDescriptorSet descriptorSet;
		const S& set;
	};
}

#endif
