#ifndef VULKAN_INSTANCE
#define VULKAN_INSTANCE

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vector>


namespace Vulkan { class Instance; }

/**
 * Class used to initialize Vulkan.
 */
class Vulkan::Instance {
	public:

		Instance(const std::string& appName = "");

		/**
		 * @brief Cleans up the instance resources.
		 */
		~Instance();

		Instance(const Instance&) = delete;
		Instance(Instance&&) = delete;
		Instance& operator=(const Instance&) = delete;
		Instance& operator=(Instance&&) = delete;
		
		const VkInstance& operator+() const;

	private:
		void checkValidationLayerSupport(std::vector<const char*> validationLayers);


		VkInstance instance;
};

#endif
