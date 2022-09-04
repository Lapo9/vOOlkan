#include "Image.h"
#include "SwapchainSurfaceFormat.h"
#include "ImageView.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanException.h"
#include "Buffer.h" //for the findSuitableMemoryType function

#include <iostream>


Vulkan::Image::Image(const VkImage& image, const LogicalDevice& virtualGpu, VkFormat format, std::pair<unsigned int, unsigned int> resolution) : image{ image }, format{ format }, virtualGpu{ &virtualGpu }, isSwapchainImage{ true }, allocatedMemory{ VK_NULL_HANDLE }, layout{ VK_IMAGE_LAYOUT_UNDEFINED }, resolution{ resolution } {
	generateImageView("base", virtualGpu);
	std::cout << "\n+ Image created";
}



Vulkan::Image::Image() : image{ VK_NULL_HANDLE }, allocatedMemory{ VK_NULL_HANDLE }, format{ VK_FORMAT_UNDEFINED }, virtualGpu{ nullptr }, isSwapchainImage{ false }, layout{ VK_IMAGE_LAYOUT_UNDEFINED }, resolution{ 0,0 }{}



Vulkan::Image::Image(Image&& movedFrom) noexcept : Image{} {
	swap(*this, movedFrom);

	std::cout << "\n> Image moved";
}



Vulkan::Image& Vulkan::Image::operator=(Image&& movedFrom) noexcept {
	Image temp{ std::move(movedFrom) };

	swap(*this, temp);

	std::cout << "\n=> Image move assigned";
	return *this;
}


Vulkan::Image::~Image() {
	if (image != VK_NULL_HANDLE && !isSwapchainImage) {
		vkDestroyImage(+*virtualGpu, image, nullptr);
	}
	if (allocatedMemory != VK_NULL_HANDLE && !isSwapchainImage) {
		vkFreeMemory(+*virtualGpu, allocatedMemory, nullptr);
	}
	std::cout << "\n- Image destroyed";
}


const VkImage& Vulkan::Image::operator+() const {
	return image;
}


VkFormat Vulkan::Image::getFormat() const {
	return format;
}



const Vulkan::ImageView& Vulkan::Image::generateImageView(std::string tag, const LogicalDevice& virtualGpu, VkImageAspectFlags type) {
	auto res = views.emplace(std::piecewise_construct,
		std::forward_as_tuple(tag),
		std::forward_as_tuple(*this, virtualGpu, type));

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




void Vulkan::swap(Image& lhs, Image& rhs) {
	using std::swap;
	swap(lhs.image, rhs.image);
	swap(lhs.format, rhs.format);
	swap(lhs.views, rhs.views);
	swap(lhs.allocatedMemory, rhs.allocatedMemory);
	swap(lhs.virtualGpu, rhs.virtualGpu);
	swap(lhs.isSwapchainImage, rhs.isSwapchainImage);
	swap(lhs.resolution, rhs.resolution);
}