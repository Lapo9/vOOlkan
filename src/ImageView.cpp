#include "ImageView.h"
#include "Image.h"
#include "LogicalDevice.h"
#include "SwapchainSurfaceFormat.h"
#include "VulkanException.h"


Vulkan::ImageView::ImageView(const Image& image, const LogicalDevice& virtualGpu) : virtualGpu{ virtualGpu } {
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



Vulkan::ImageView::~ImageView() {
	vkDestroyImageView(+virtualGpu, imageView, nullptr);
}



const VkImageView& Vulkan::ImageView::operator+() const {
	return imageView;
}
