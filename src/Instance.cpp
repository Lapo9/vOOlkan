#include "Instance.h"
#include "VulkanException.h"

#include <iostream>


Vulkan::Instance::Instance(const std::string& appName) {
	//struct containing info about our application
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "fff";// appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//get the extensions needed for Vulkan to work with the OS specific window system
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//struct containing the extensions we want to use
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	//actually create the Vulkan instance
	if (const auto result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
		throw VulkanException("Failed to create instance!", result);
	}

	std::cout << "\n+ Instance created";
}



Vulkan::Instance::~Instance() {
	vkDestroyInstance(instance, nullptr);
	std::cout << "\n- Instance destroyed";
}



const VkInstance& Vulkan::Instance::operator+() const {
	return instance;
}
