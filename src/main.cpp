#include <vulkan/vulkan.h>
#include <iostream>

#include "Pinball.h"

int main() {
	try {
		Vulkan::Window window{ 500, 300, "Test app title" };
		Vulkan::Instance vulkanInstance{ "test app" };
		Vulkan::WindowSurface windowSurface{ vulkanInstance, window };
		Vulkan::PhysicalDevice realGpu{ vulkanInstance, windowSurface };
		Vulkan::LogicalDevice virtualGpu{ realGpu };
		Vulkan::Swapchain swapchain{ realGpu, virtualGpu, windowSurface, window };

		Vulkan::PipelineOptions::RenderPassOptions::Attachment a{};
		Vulkan::PipelineOptions::RenderPassOptions::Attachment b{};
		Vulkan::PipelineOptions::RenderPassOptions::Attachment c{};

		std::pair pa{ a, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR };
		std::pair pb{ b, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR };
		std::pair pc{ c, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR };

		auto z = Vulkan::PipelineOptions::RenderPassOptions::Attachment::prepareAttachmentsArray(pa, pb, c);
		std::cout << "\n";
	}
	catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}

	std::cout << "\n\n";
}
