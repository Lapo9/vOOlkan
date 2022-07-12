#ifndef VULKAN_PHYSICALDEVICE
#define VULKAN_PHYSICALDEVICE

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <functional>

#include "Instance.h"
#include "VulkanException.h"
#include "classes_to_do.h" //REMOVE when done


namespace Vulkan { class PhysicalDevice; enum class QueueFamily; }

/**
 * @brief Represents the GPU (or any other device) that will be used with Vulkan to perform computer graphics.
 */
class Vulkan::PhysicalDevice {
	public:
		PhysicalDevice(const Instance& vulkanInstance, const WindowSurface& surface) {
			pickBestDevice(vulkanInstance, surface);
		}

		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice(PhysicalDevice&&) = delete;
		PhysicalDevice& operator=(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(PhysicalDevice&&) = delete;

		/**
		 * @brief Returns the underlying VkPhyisicalDevice object.
		 * @return The underlying VkPhyisicalDevice object.
		 */
		const VkPhysicalDevice& operator+() const {
			return gpu;
		}


		/**
		 * @brief Returns the indices of the graphics and presentation queue families (they can be the same family).
		 * @return The indices of the graphics and presentation queue families.
		 */
		std::map<QueueFamily, int> getQueueFamiliesIndices() const {
			return queueFamiliesIndices;
		}



	private:
		void pickBestDevice(const Instance& vulkanInstance, const WindowSurface& surface) {
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
				if (isGpuSuitable(gpu, surface)) {
					this->gpu = gpu; //set the GPU we'll use
					this->queueFamiliesIndices = queueFamiliesSupport(gpu, surface); //save the indices of the graphics and present queue
					break;
				}
			}

			if (this->gpu == VK_NULL_HANDLE) {
				throw VulkanException("Failed to find a GPU suitable for Vulkan!");
			}
		}

		//given a gpu tell me if it supports all the required features
		static bool isGpuSuitable(const VkPhysicalDevice& gpu, const WindowSurface& surface) {
			try{
				queueFamiliesSupport(gpu, surface);
				extensionsSupport(gpu);
			} catch (const VulkanException&) {
				return false;
			}
			return true;
		}


		//Returns the index of the graphics and presentation families if supported by the gpu, else it will throw.
		static std::map<QueueFamily, int> queueFamiliesSupport(const VkPhysicalDevice& gpu, const WindowSurface& surface) {
			//get all the queue families supported by this GPU
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilies.data());

			//for each family, check whether it supports the required families (families are named by their position in the queueFamily vector
			std::map<QueueFamily, std::vector<int>> supportedFamilies; //std::map[families supporting graphics, families supporting presentation]
			for (int i = 0;  const auto & queueFamily : queueFamilies) {
				//check graphics family
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					supportedFamilies[QueueFamily::GRAPHICS].push_back(i);
				}

				//check presentation family
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, +surface, &presentSupport);
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


		static void extensionsSupport(VkPhysicalDevice gpu) {
			//TODO
		}


		//given a tuple of vectors containing ints, choose one int from each vector, trying to choose as many elements in common among the vectors as possible
		static std::map<QueueFamily, int> chooseFewestPossible(std::map<QueueFamily, std::vector<int>> families) {
			for (int i : families[QueueFamily::GRAPHICS]) {
				for (int j : families[QueueFamily::PRESENTATION]) {
					if (i == j) {
						return std::map<QueueFamily, int>{ {QueueFamily::GRAPHICS, i}, { QueueFamily::PRESENTATION, j } };
					}
				}
			}

			return std::map<QueueFamily, int>{ {QueueFamily::GRAPHICS, families[QueueFamily::GRAPHICS][0]}, { QueueFamily::PRESENTATION, families[QueueFamily::PRESENTATION][0] } };
		}


		VkPhysicalDevice gpu = VK_NULL_HANDLE;
		std::map<QueueFamily, int> queueFamiliesIndices; //the indices of the graphics and presentation families (they can be the same)
};


/**
 * @brief Types of queue families.
 */
enum class Vulkan::QueueFamily {
	GRAPHICS, PRESENTATION
};

#endif
