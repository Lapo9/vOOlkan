#include "PhysicalDevice.h"
#include "Instance.h"
#include "WindowSurface.h"
#include "Swapchain.h"
#include "QueueFamily.h"
#include "VulkanException.h"

#include <iostream>


Vulkan::PhysicalDevice::PhysicalDevice(const Instance& vulkanInstance, const WindowSurface& surface) {
	pickBestDevice(vulkanInstance, surface);
	std::cout << "\n+ PhysicalDevice created";
}



const VkPhysicalDevice& Vulkan::PhysicalDevice::operator+() const {
	return gpu;
}



std::map<Vulkan::QueueFamily, int> Vulkan::PhysicalDevice::getQueueFamiliesIndices() const {
	return queueFamiliesIndices;
}



std::vector<const char*> Vulkan::PhysicalDevice::getRequiredDeviceExtensions() const {
	return requiredDeviceExtensions;
}



Vulkan::PhysicalDevice::PhysicalDevice(const VkPhysicalDevice& underlyingGpu) : gpu{ underlyingGpu } {}



void Vulkan::PhysicalDevice::pickBestDevice(const Instance& vulkanInstance, const WindowSurface& surface) {
	//get all of the GPUs we have in the system
	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(+vulkanInstance, &gpuCount, nullptr);
	if (gpuCount == 0) {
		throw VulkanException("Failed to find any GPU!");
	}
	std::vector<VkPhysicalDevice> gpus(gpuCount);
	vkEnumeratePhysicalDevices(+vulkanInstance, &gpuCount, gpus.data());

	//for each GPU find out whether it is suitable, if so, set it as the GPU we'll use
	//TODO maybe rank GPUs from best to worse
	for (const auto& gpu : gpus) {
		if (const PhysicalDevice gpuObject{ gpu }; isGpuSuitable(gpuObject, surface, requiredDeviceExtensions)) {
			this->gpu = gpu; //set the GPU we'll use
			this->queueFamiliesIndices = queueFamiliesSupport(gpuObject, surface); //save the indices of the graphics and present queue
			break;
		}
	}

	if (this->gpu == VK_NULL_HANDLE) {
		throw VulkanException("Failed to find a GPU suitable for Vulkan!");
	}
}



bool Vulkan::PhysicalDevice::isGpuSuitable(const PhysicalDevice& gpu, const WindowSurface& surface, const std::vector<const char*> requiredDeviceExtensions) {
	try {
		queueFamiliesSupport(gpu, surface);
		extensionsSupport(gpu, requiredDeviceExtensions);
	} catch (const VulkanException&) {
		return false;
	}
	return Swapchain::isSwapchainSupported(gpu, surface); //if it supports the queue families and the extensions, check whether it supports a swapchain for this window surface
}



std::map<Vulkan::QueueFamily, int> Vulkan::PhysicalDevice::queueFamiliesSupport(const PhysicalDevice& gpu, const WindowSurface& surface) {
	//get all the queue families supported by this GPU
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(+gpu, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(+gpu, &queueFamilyCount, queueFamilies.data());

	//for each family, check whether it supports the required families (families are named by their position in the queueFamily vector
	std::map<QueueFamily, std::vector<int>> supportedFamilies; //std::map[families supporting graphics, families supporting presentation]
	for (int i = 0; const auto & queueFamily : queueFamilies) {
		//check graphics family
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			supportedFamilies[QueueFamily::GRAPHICS].push_back(i);
		}

		//check presentation family
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(+gpu, i, +surface, &presentSupport);
		if (presentSupport) {
			supportedFamilies[QueueFamily::PRESENTATION].push_back(i);
		}

		i++;
	}

	//if all the required features are supported by the same queue family, then choose that queue family, else choose different queue families
	if (supportedFamilies[QueueFamily::GRAPHICS].size() == 0 || supportedFamilies[QueueFamily::PRESENTATION].size() == 0) {
		throw VulkanException("The GPU doesn't support all of the required families");
	}
	return chooseFewestPossible(supportedFamilies);
}



void Vulkan::PhysicalDevice::extensionsSupport(const PhysicalDevice& gpu, const std::vector<const char*>& requiredDeviceExtensions) {
	//get all of the available extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(+gpu, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(+gpu, nullptr, &extensionCount, availableExtensions.data());

	//add all the required extensions to a set, then eliminate from this set the extensions which are avalable in this GPU
	std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	//if the set is empty it means that all of the required extensions are present; if not, throw
	if (!requiredExtensions.empty()) {
		std::string exceptionMessage{ "The GPU doesn't support all of the required extensions: " };
		for (const auto& e : requiredExtensions) {
			exceptionMessage += e.c_str();
			exceptionMessage += ", ";
		}
		throw VulkanException(exceptionMessage);
	}
}



std::map<Vulkan::QueueFamily, int> Vulkan::PhysicalDevice::chooseFewestPossible(std::map<QueueFamily, std::vector<int>> families) {
	for (int i : families[QueueFamily::GRAPHICS]) {
		for (int j : families[QueueFamily::PRESENTATION]) {
			if (i == j) {
				return std::map<QueueFamily, int>{ {QueueFamily::GRAPHICS, i}, { QueueFamily::PRESENTATION, j } };
			}
		}
	}

	return std::map<QueueFamily, int>{ {QueueFamily::GRAPHICS, families[QueueFamily::GRAPHICS][0]}, { QueueFamily::PRESENTATION, families[QueueFamily::PRESENTATION][0] } };
}
