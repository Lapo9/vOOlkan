#include "Image.h"
#include "SwapchainSurfaceFormat.h"
#include "ImageView.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanException.h"
#include "Buffer.h" //for the findSuitableMemoryType function

#include <iostream>


Vulkan::Image::Image(const VkImage& image, const LogicalDevice& virtualGpu, VkFormat format) : image{ image }, format{ format }, virtualGpu{ &virtualGpu }, isSwapchainImage{ true }, allocatedMemory{ VK_NULL_HANDLE } {
	generateImageView("base", virtualGpu);
	std::cout << "\n+ Image created";
}



Vulkan::Image::Image() : image{ VK_NULL_HANDLE }, allocatedMemory{ VK_NULL_HANDLE }, format{ VK_FORMAT_UNDEFINED }, virtualGpu{ nullptr }, isSwapchainImage{ false }{}



Vulkan::Image::Image(Image&& movedFrom) noexcept : Image{} {
	std::swap(image, movedFrom.image);
	std::swap(format, movedFrom.format);
	std::swap(views, movedFrom.views);
	std::swap(allocatedMemory, movedFrom.allocatedMemory);
	std::swap(virtualGpu, movedFrom.virtualGpu);
	std::swap(isSwapchainImage, movedFrom.isSwapchainImage);

	std::cout << "\n> Image moved";
}



Vulkan::Image& Vulkan::Image::operator=(Image&& movedFrom) noexcept {
	Image temp{ std::move(movedFrom) };

	std::swap(image, temp.image);
	std::swap(format, temp.format);
	std::swap(views, temp.views);
	std::swap(allocatedMemory, temp.allocatedMemory);
	std::swap(virtualGpu, temp.virtualGpu);
	std::swap(isSwapchainImage, movedFrom.isSwapchainImage);

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