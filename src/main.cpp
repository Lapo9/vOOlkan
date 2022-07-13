#include <vulkan/vulkan.h>
#include <iostream>

#include "Pinball.h"

int main() {
	try {
		Vulkan::Instance vulkanInstance{ "test app" };
		Vulkan::Window window{ 500, 300, "Test app title" };
		Vulkan::WindowSurface windowSurface{ vulkanInstance, window };
		Vulkan::PhysicalDevice realGpu{ vulkanInstance, windowSurface };
		Vulkan::LogicalDevice virtualGpu{ realGpu };
		Vulkan::Swapchain swapchain{ realGpu, virtualGpu, windowSurface, window };
	}
	catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}

	std::cout << "\n\n";
}
