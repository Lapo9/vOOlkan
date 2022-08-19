#ifndef VULKAN_BARRIERS
#define VULKAN_BARRIERS

#include <vulkan/vulkan.h>


namespace Vulkan::SynchronizationPrimitives { class ImageMemoryBarrier; }

class Vulkan::SynchronizationPrimitives::ImageMemoryBarrier {
public:

	ImageMemoryBarrier(const VkImage& image, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) {
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;
	}


	const VkImageMemoryBarrier& operator+() const {
		return barrier;
	}

private:
	VkImageMemoryBarrier barrier;

};

#endif
