#include "Swapchain.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "Image.h"
#include "WindowSurface.h"
#include "SwapchainCapabilities.h"
#include "SwapchainPresentMode.h"
#include "SwapchainSurfaceFormat.h"
#include "QueueFamily.h"
#include "VulkanException.h"

#include <iostream>


Vulkan::Swapchain::Swapchain(const PhysicalDevice& realGpu, const LogicalDevice& virtualGpu, const WindowSurface& windowSurface, const Window& window) :
	swapchainCapabilities{ realGpu, windowSurface },
	swapchainSurfaceFormat{ realGpu, windowSurface },
	swapchainPresentMode{ realGpu, windowSurface },
	virtualGpu{ &virtualGpu } {
	create(realGpu, virtualGpu, windowSurface, window);

	std::cout << "\n+ Swapchain created";
}


Vulkan::Swapchain::~Swapchain() {
	vkDestroySwapchainKHR(+*virtualGpu, swapchain, nullptr);
	std::cout << "\n- Swapchain destroyed";
}



Vulkan::Swapchain& Vulkan::Swapchain::operator=(Swapchain&& movedFrom) noexcept {
	std::swap(swapchain, movedFrom.swapchain);
	std::swap(swapchainCapabilities, movedFrom.swapchainCapabilities);
	std::swap(swapchainSurfaceFormat, movedFrom.swapchainSurfaceFormat);
	std::swap(swapchainPresentMode, movedFrom.swapchainPresentMode);
	std::swap(images, movedFrom.images);
	std::swap(virtualGpu, movedFrom.virtualGpu);

	std::cout << "\n=> Swapchain move assigned";
	return *this;
}



const VkSwapchainKHR& Vulkan::Swapchain::operator+() const {
	return swapchain;
}


const Vulkan::SwapchainOptions::SurfaceFormat& Vulkan::Swapchain::getImageFormat() {
	return swapchainSurfaceFormat;
}

const Vulkan::SwapchainOptions::Capabilities& Vulkan::Swapchain::getSwapchainCapabilities() {
	return swapchainCapabilities;
}


const std::vector<Vulkan::Image>& Vulkan::Swapchain::getImages() const {
	return images;
}


std::pair<unsigned int, unsigned int> Vulkan::Swapchain::getResolution() const {
	return { (+swapchainCapabilities).currentExtent.width, (+swapchainCapabilities).currentExtent.height };
}


bool Vulkan::Swapchain::isSwapchainSupported(const PhysicalDevice& realGpu, const WindowSurface& windowSurface) {
	return SwapchainOptions::SurfaceFormat::isThereAnAvailableSurfaceFormat(realGpu, windowSurface) && SwapchainOptions::PresentMode::isThereAnAvailablePresentMode(realGpu, windowSurface);
}


void Vulkan::Swapchain::saveSwapchainImages() {
	std::vector<VkImage> tmpImages;
	uint32_t imageCount;
	vkGetSwapchainImagesKHR(+*virtualGpu, swapchain, &imageCount, nullptr);
	tmpImages.resize(imageCount);
	vkGetSwapchainImagesKHR(+*virtualGpu, swapchain, &imageCount, tmpImages.data());

	for (const auto& image : tmpImages) {
		images.emplace_back(image, *virtualGpu, swapchainSurfaceFormat);
	}
}


void Vulkan::Swapchain::recreate(const PhysicalDevice& realGpu, const LogicalDevice& virtualGpu, const WindowSurface& windowSurface, const Window& window) {
	vkDestroySwapchainKHR(+*this->virtualGpu, swapchain, nullptr);
	images.clear();

	swapchainCapabilities = SwapchainOptions::Capabilities{ realGpu, windowSurface };
	swapchainSurfaceFormat = SwapchainOptions::SurfaceFormat{ realGpu, windowSurface };
	swapchainPresentMode = SwapchainOptions::PresentMode{ realGpu, windowSurface };
	this->virtualGpu = &virtualGpu;

	create(realGpu, virtualGpu, windowSurface, window);

	std::cout << "\n# Swapchain re-created";
}


void Vulkan::Swapchain::create(const PhysicalDevice& realGpu, const LogicalDevice& virtualGpu, const WindowSurface& windowSurface, const Window& window) {
	//struct to create the swapchain with the specified properties
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = +windowSurface;
	createInfo.minImageCount = std::clamp((+swapchainCapabilities).minImageCount + 1, (+swapchainCapabilities).minImageCount, (+swapchainCapabilities).maxImageCount); //we want 1 more image in the swap chain than the minimum in order not to wait if all images are occupied
	createInfo.imageFormat = (+swapchainSurfaceFormat).format;
	createInfo.imageColorSpace = (+swapchainSurfaceFormat).colorSpace;
	createInfo.imageExtent = swapchainCapabilities.chooseSwapExtent(window);
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = (+swapchainCapabilities).currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = +swapchainPresentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	//if the queue families for the graphics and presentation queues are different, then whe have to use a particular option
	uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(realGpu.getQueueFamiliesIndices()[QueueFamily::GRAPHICS]), static_cast<uint32_t>(realGpu.getQueueFamiliesIndices()[QueueFamily::PRESENTATION]) };
	if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	//actually create the swapchain with the specified options
	if (auto result = vkCreateSwapchainKHR(+virtualGpu, &createInfo, nullptr, &swapchain); result != VK_SUCCESS) {
		throw VulkanException("Failed to create swap chain!", result);
	}

	//save the images of the swap chain to be able to access them later on
	saveSwapchainImages();
}
