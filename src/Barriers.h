#ifndef VULKAN_BARRIERS
#define VULKAN_BARRIERS

#include <vulkan/vulkan.h>

#include "Image.h"
#include "VulkanException.h"


namespace Vulkan::SynchronizationPrimitives { class ImageMemoryBarrier; }

class Vulkan::SynchronizationPrimitives::ImageMemoryBarrier {
public:

	ImageMemoryBarrier(const Image& image, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) {
		auto maskInfo = getTransitionInfo(oldLayout, newLayout);
		
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = +image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = maskInfo.sourceMask;
		barrier.dstAccessMask = maskInfo.destinationMask;
	}


	const VkImageMemoryBarrier& operator+() const {
		return barrier;
	}



	struct TransitionInfo {
		VkPipelineStageFlagBits sourceStage, destinationStage;
		VkAccessFlagBits sourceMask, destinationMask;
	};



	/**
	 * @brief Returns the right values for the mask and source/destination stage for a transition based on the old and new layouts.
	 */
	static TransitionInfo getTransitionInfo(const VkImageLayout& oldLayout, const VkImageLayout& newLayout) {
		VkPipelineStageFlagBits sourceStage, destinationStage;
		VkAccessFlagBits sourceMask, destinationMask;
		
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			sourceMask = VK_ACCESS_NONE;
			destinationMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			sourceMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw VulkanException("Unsupported layout transition!");
		}

		return MaskInfo{ sourceStage, destinationStage, sourceMask, destinationMask };
	}


private:
	VkImageMemoryBarrier barrier;

};

#endif
