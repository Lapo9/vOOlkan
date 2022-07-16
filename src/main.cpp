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

		Vulkan::PipelineOptions::RenderPassOptions::Attachment a1{}; std::pair pa1{ a1, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR };
		Vulkan::PipelineOptions::RenderPassOptions::Attachment a2{}; std::pair pa2{ a2, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR };
		Vulkan::PipelineOptions::RenderPassOptions::Attachment a3{}; std::pair pa3{ a3, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR };

		auto boundAttachments = Vulkan::PipelineOptions::RenderPassOptions::Attachment::prepareAttachments(pa1, a2, std::pair{ a3, VkImageLayout::VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR });

		Vulkan::PipelineOptions::RenderPassOptions::Subpass s1(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, boundAttachments[0], std::pair{ boundAttachments[1], true });
		Vulkan::PipelineOptions::RenderPassOptions::Subpass s2(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, std::pair{ boundAttachments[2], true }, boundAttachments[0]);

		Vulkan::PipelineOptions::RenderPass renderPass{ virtualGpu, boundAttachments, s1,s2 };

		using MyVertex = Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec2>;
		std::vector<MyVertex> v1s{
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		};

		auto [binding, attributes] = MyVertex::getDescriptors(0);

		std::cout << "\n";
	} catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}

	std::cout << "\n\n";
}
