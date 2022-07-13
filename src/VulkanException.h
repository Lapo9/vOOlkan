#ifndef VULKAN_EXCEPTION
#define VULKAN_EXCEPTION

#include <stdexcept>
#include <string>
#include <vulkan/vulkan.h>

namespace Vulkan { class VulkanException; }

/**
 * @brief Generic runtime exception thrown by Vulkan-related functions.
 */
class Vulkan::VulkanException : public std::runtime_error {
	public:
		VulkanException(const std::string& description = defaultDescription, VkResult errorCode = VK_SUCCESS) : std::runtime_error(description) {
			this->errorCode = errorCode;
		}

		const char* what() const noexcept override{
			fullDescription = "\n- Vulkan runtime error";
			if(errorCode != VK_SUCCESS) {
				fullDescription += " #" + std::to_string(errorCode);
			}
			if (const std::string description = std::runtime_error::what(); description != defaultDescription) {
				fullDescription += ": " + description;
			}
			return fullDescription.c_str();
		}

	private:
		VkResult errorCode;
		mutable std::string fullDescription;
		inline static const std::string defaultDescription = "no_description_provided";
};



#endif
