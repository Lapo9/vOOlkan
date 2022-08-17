#include "ImageView.h"
#include "Image.h"
#include "LogicalDevice.h"
#include "SwapchainSurfaceFormat.h"
#include "VulkanException.h"

#include <iostream>


Vulkan::ImageView::ImageView(const Image& image, const LogicalDevice& virtualGpu, VkImageAspectFlags type) : virtualGpu{ virtualGpu } {
	//struct to create an image view for this image
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = +image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = image.getFormat();
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = type;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	//actually create the image view
	if (auto result = vkCreateImageView(+virtualGpu, &createInfo, nullptr, &imageView); result != VK_SUCCESS) {
		throw VulkanException("Failed to create image view!", result);
	}

	std::cout << "\n+ ImageView created";
}



Vulkan::ImageView::~ImageView() {
	vkDestroyImageView(+virtualGpu, imageView, nullptr);
	std::cout << "\n- ImageView destroyed";
}



const VkImageView& Vulkan::ImageView::operator+() const {
	return imageView;
}
