#ifndef VULKAN_PHYSICALDEVICE
#define VULKAN_PHYSICALDEVICE

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <set>
#include <functional>


namespace Vulkan { class PhysicalDevice; class Instance; class WindowSurface; enum class QueueFamily; }


/**
 * @brief Represents the GPU (or any other device) that will be used with Vulkan to perform computer graphics.
 */
class Vulkan::PhysicalDevice {
	public:
		PhysicalDevice(const Instance& vulkanInstance, const WindowSurface& surface);


		/**
		 * @brief Returns the underlying VkPhyisicalDevice object.
		 * @return The underlying VkPhyisicalDevice object.
		 */
		const VkPhysicalDevice& operator+() const;


		/**
		 * @brief Returns the indices of the graphics and presentation queue families (they can be the same family).
		 * @return The indices of the graphics and presentation queue families.
		 */
		std::map<QueueFamily, int> getQueueFamiliesIndices() const;

		/**
		 * Returns the list of required GPU extensions.
		 * 
		 * @return The list of required GPU extensions.
		 */
		std::vector<const char*> getRequiredDeviceExtensions() const;


		VkPhysicalDeviceProperties getProperties() const {
			VkPhysicalDeviceProperties limits;
			vkGetPhysicalDeviceProperties(gpu, &limits);
			return limits;
		}


	private:
		//constructor only used to pass around PhysicalDevices to functions which require a PhysicalDevice instead of a VkPhysicalDevice
		PhysicalDevice(const VkPhysicalDevice& underlyingGpu);


		void pickBestDevice(const Instance& vulkanInstance, const WindowSurface& surface);

		//given a gpu tell me if it supports all the required features
		static bool isGpuSuitable(const PhysicalDevice& gpu, const WindowSurface& surface, const std::vector<const char*> requiredDeviceExtensions);


		//Returns the index of the graphics and presentation families if supported by the GPU, else it will throw.
		static std::map<QueueFamily, int> queueFamiliesSupport(const PhysicalDevice& gpu, const WindowSurface& surface);


		//Checks whether all of the required extensions are supported. If not it will throw.
		static void extensionsSupport(const PhysicalDevice& gpu, const std::vector<const char*>& requiredDeviceExtensions);


		//given a tuple of vectors containing ints, choose one int from each vector, trying to choose as many elements in common among the vectors as possible
		static std::map<QueueFamily, int> chooseFewestPossible(std::map<QueueFamily, std::vector<int>> families);


		VkPhysicalDevice gpu = VK_NULL_HANDLE;
		std::map<QueueFamily, int> queueFamiliesIndices; //the indices of the graphics and presentation families (they can be the same)
		const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; //list of required extensions for the device
};

#endif
