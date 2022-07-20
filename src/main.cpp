#include <vulkan/vulkan.h>
#include <iostream>

#include "Pinball.h"

int main() {
	try {
		//GPU setup
		Vulkan::Window window{ 500, 300, "Test app title" };
		Vulkan::Instance vulkanInstance{ "test app" };
		Vulkan::WindowSurface windowSurface{ vulkanInstance, window };
		Vulkan::PhysicalDevice realGpu{ vulkanInstance, windowSurface };
		Vulkan::LogicalDevice virtualGpu{ realGpu };
		Vulkan::Swapchain swapchain{ realGpu, virtualGpu, windowSurface, window };


		//attachments for the render pass of the pipeline
		Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription a1{};
		Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription a2{};
		Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription a3{};

		//attachments ready to be used in a render pass's subpass
		auto boundAttachments = Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription::prepareAttachments(a1);//, a2, std::pair{ a3, VkImageLayout::VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR });

		//subpasses
		Vulkan::PipelineOptions::RenderPassOptions::Subpass s1(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, boundAttachments[0]);
		//Vulkan::PipelineOptions::RenderPassOptions::Subpass s2(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, std::pair{ boundAttachments[2], true }, boundAttachments[0]);

		//render pass
		Vulkan::PipelineOptions::RenderPass renderPass{ virtualGpu, boundAttachments, s1};

		//how a vertex is made up of
		using MyVertex = Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec2>;
		{
			//vertex array example
			std::vector<MyVertex> v1s{
				{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
				{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
			};
		}

		//vertices types descriptor
		Vulkan::PipelineOptions::PipelineVertexArrays vertexTypesDescriptor{ MyVertex{} };

		//pipeline options
		Vulkan::PipelineOptions::Multisampler multisampler{};
		Vulkan::PipelineOptions::DepthStencil depthStencil{};
		Vulkan::PipelineOptions::DynamicState dynamicState{};
		Vulkan::PipelineOptions::InputAssembly inputAssembly{};
		Vulkan::PipelineOptions::PipelineLayout pipelineLayout{virtualGpu};
		Vulkan::PipelineOptions::Rasterizer rasterizer{};
		Vulkan::PipelineOptions::Viewport viewport{};

		//shaders
		Vulkan::PipelineOptions::Shader vertexShader{ virtualGpu, "shaders/TestVert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT };
		Vulkan::PipelineOptions::Shader fragmentShader{ virtualGpu, "shaders/TestFrag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT };

		//pipeline
		Vulkan::Pipeline pipeline{ virtualGpu, renderPass, 0, std::vector{&vertexShader, &fragmentShader},vertexTypesDescriptor, pipelineLayout, inputAssembly, rasterizer, multisampler, depthStencil, dynamicState, viewport};


		//create the framebuffers
		auto framebuffers = Vulkan::Framebuffer::generateFramebufferForEachSwapchainImageView(virtualGpu, renderPass, swapchain);

		Vulkan::CommandBufferPool commandBufferPool{ virtualGpu };

		uint32_t a = 1;
		Vulkan::CommandBuffer commandBuffer{ virtualGpu, commandBufferPool, renderPass, framebuffers[0], pipeline, std::tuple{vkCmdDraw, a, a, a, a}, std::tuple{vkCmdDraw, a, a, a, a}};;

		std::cout << "\n";
	} catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}

	std::cout << "\n\n";
}
