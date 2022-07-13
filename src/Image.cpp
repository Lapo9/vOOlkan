#include "Image.h"
#include "SwapchainSurfaceFormat.h"
#include "ImageView.h"
#include "VulkanException.h"


Vulkan::Image::Image(const VkImage& image, const SwapchainOptions::SurfaceFormat& format) : image{ image }, format{ format } {
}


const VkImage& Vulkan::Image::operator+() const {
	return image;
}


const Vulkan::SwapchainOptions::SurfaceFormat& Vulkan::Image::getFormat() const {
	return format;
}



Vulkan::ImageView& Vulkan::Image::generateImageView(std::string tag, const LogicalDevice& virtualGpu) {
	auto res = views.emplace(std::piecewise_construct,
		std::forward_as_tuple(tag),
		std::forward_as_tuple(*this, virtualGpu));

	if (!res.second) {
		throw VulkanException("Cannot create an image view with tag " + tag + " because the tag has been already used");
	}
	return res.first->second;
}



Vulkan::ImageView& Vulkan::Image::operator[](std::string tag) {
	auto result = views.find(tag);
	if (result == views.end()) {
		throw VulkanException("Image view with tag " + tag + " doesn't exist");
	}
	return result->second;
}
