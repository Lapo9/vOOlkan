#ifndef VULKAN_DRAWER
#define VULKAN_DRAWER

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <string>

#include "LogicalDevice.h"
#include "Swapchain.h"
#include "Pipeline.h"


namespace Vulkan { class Drawer; class Semaphore; class Fence; }


/**
 * @brief A Drawer is a class which holds all of the resources to draw frames on screen, such as the synchronization primitives, the framebuffers and the function to actually draw a frame.
 */
class Vulkan::Drawer {
public:
	Drawer(const LogicalDevice& virtualGpu) {}

	void draw()
	

private:
	//Each string (name) is bound to a vector of synch primitives. Each element in the vector is a different primitive, and each one is used for a specific frame in flight.
	//In reality each name is bound to a pair, where the second element is the actual primitive hold by the first one, for performance reasons.
	std::map<std::string, std::pair<std::vector<Semaphore>, std::vector<VkSemaphore>>> semaphores;
	std::map<std::string, std::pair<std::vector<Fence>, std::vector<VkFence>>> fences;
};

#endif
