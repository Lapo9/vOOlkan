#ifndef VULKAN_INSTANCE
#define VULKAN_INSTANCE

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>

#include "VulkanException.h"


namespace Vulkan { class Instance; }

/**
 * Class used to initialize Vulkan.
 */
class Vulkan::Instance {
	public:

		Instance(const std::string& appName = "") {
			//struct containing info about our application
			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = appName.c_str();
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

			//actually create the Vulkan instance
			if (const auto result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
				throw VulkanException("Failed to create instance!", result);
			}
		}

		/**
		 * @brief Cleans up the instance resources.
		 */
		~Instance() {
			vkDestroyInstance(instance, nullptr);
		}

		Instance(Instance&) = delete;
		Instance(Instance&&) = delete;
		Instance& operator=(Instance&) = delete;
		Instance& operator=(Instance&&) = delete;
		
		const VkInstance operator+() const {
			return instance;
		}

	private:
		VkInstance instance;
};

#endif
