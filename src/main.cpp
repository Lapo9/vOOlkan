#include <vulkan/vulkan.h>

#include "Pinball.h"

int main() {
	Vulkan::Instance vulkanInstance{ "test app" };
	Vulkan::Window window{ 500, 300, "Test app title" };
	Vulkan::WindowSurface windowSurface{ vulkanInstance, window };
	Vulkan::PhysicalDevice realGpu{ vulkanInstance, windowSurface };
	Vulkan::LogicalDevice virtualGpu{ realGpu };
	Vulkan::Swapchain swapchain{ realGpu, virtualGpu, windowSurface, window };

}
