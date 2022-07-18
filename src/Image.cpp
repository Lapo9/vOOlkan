#include "Image.h"
#include "SwapchainSurfaceFormat.h"
#include "ImageView.h"
#include "VulkanException.h"

#include <iostream>


Vulkan::Image::Image(const VkImage& image, const LogicalDevice& virtualGpu, const SwapchainOptions::SurfaceFormat& format) : image{ image }, format{ format } {
	generateImageView("base", virtualGpu);
	std::cout << "\n+ Image created";
}


const VkImage& Vulkan::Image::operator+() const {
	return image;
}


const Vulkan::SwapchainOptions::SurfaceFormat& Vulkan::Image::getFormat() const {
	return format;
}



const Vulkan::ImageView& Vulkan::Image::generateImageView(std::string tag, const LogicalDevice& virtualGpu) {
	auto res = views.emplace(std::piecewise_construct,
		std::forward_as_tuple(tag),
		std::forward_as_tuple(*this, virtualGpu));

	if (!res.second) {
		throw VulkanException("Cannot create an image view with tag " + tag + " because the tag has been already used", "Change tag");
	}
	return res.first->second;
}


void Vulkan::Image::eliminateImageView(std::string tag) {
	views.erase(tag);
}


const Vulkan::ImageView& Vulkan::Image::operator[](std::string tag) const {
	auto result = views.find(tag);
	if (result == views.end()) {
		throw VulkanException("Image view with tag " + tag + " doesn't exist");
	}
	return result->second;
}



const std::map<std::string, Vulkan::ImageView>& Vulkan::Image::getImageViews() const {
	return views;
}
