#ifndef VULKAN_IMAGEVIEW
#define VULKAN_IMAGEVIEW

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"
#include "Image.h"
#include "VulkanException.h"


namespace Vulkan { class ImageView; }

/**
 * @brief Part of an image where we can actually draw.
 */
class Vulkan::ImageView {
public:

	ImageView(const Image& image, const LogicalDevice& virtualGpu) : virtualGpu{ virtualGpu } {
		//struct to create an image view for this image
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = +image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = (+image.getFormat()).format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		//actually create the image view
		if (auto result = vkCreateImageView(+virtualGpu, &createInfo, nullptr, &imageView); result != VK_SUCCESS) {
			throw VulkanException("Failed to create image view!", result);
		}
	}

	~ImageView() {
		vkDestroyImageView(+virtualGpu, imageView, nullptr);
	}

	ImageView(const ImageView&) = delete;
	ImageView(ImageView&&) = delete;
	ImageView& operator=(const ImageView&) = delete;
	ImageView& operator=(ImageView&&) = delete;

	const VkImageView& operator+() const {
		return imageView;
	}

private:
	VkImageView imageView;
	const LogicalDevice& virtualGpu;
};

#endif
