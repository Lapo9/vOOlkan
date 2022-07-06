#ifndef VULKAN_EXCEPTION
#define VULKAN_EXCEPTION

#include <stdexcept>
#include <string>
#include <vulkan/vulkan.h>

namespace Vulkan { class VulkanException; }

/**
 * @brief Generic runtime exception thrown by Vulkan.
 */
class VulkanException : public std::runtime_error {
	public:
		VulkanException(VkResult errorCode, const std::string& description = defaultDescription) : std::runtime_error(description) {
			this->errorCode = errorCode;
		}

		const char* what() const noexcept override{
			const auto description = runtime_error::what();
			return description != defaultDescription ? "Vulkan runtime error #" + std::to_string(errorCode) + ": " + description;
		}

	private:
		VkResult errorCode;
		inline static const std::string defaultDescription = "no_description_provided";
};



#endif
