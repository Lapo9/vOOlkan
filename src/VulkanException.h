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
		VulkanException(const std::string& description, VkResult errorCode, const std::string& hint = "") :
			std::runtime_error(description), 
			errorCode{ errorCode }, 
			hint{ hint } {
		}

		VulkanException(const std::string& description, const std::string& hint) :
			std::runtime_error(description),
			errorCode{ VK_SUCCESS },
			hint{ hint } {
		}

		VulkanException(const std::string& description) :
			std::runtime_error(description),
			errorCode{ VK_SUCCESS },
			hint{ "" } {
		}

		const char* what() const noexcept override{
			fullDescription = "\n- Vulkan runtime error";
			if(errorCode != VK_SUCCESS) {
				fullDescription += "\n\tcode: " + std::to_string(errorCode);
			}
			if (const std::string description = std::runtime_error::what(); description != "") {
				fullDescription += "\n\tdescription: " + description;
			}
			if (hint != "") {
				fullDescription += "\n\thint: " + hint;
			}
			return fullDescription.c_str();
		}

	private:
		VkResult errorCode;
		std::string hint;
		mutable std::string fullDescription;
};



#endif
