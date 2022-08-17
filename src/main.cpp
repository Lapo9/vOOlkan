#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>


#include "Pinball.h"




int main() {
	try {
		//GPU setup
		Vulkan::Window window{ 1000, 600, "Test app title" };
		Vulkan::Instance vulkanInstance{ "test app" };
		Vulkan::WindowSurface windowSurface{ vulkanInstance, window };
		Vulkan::PhysicalDevice realGpu{ vulkanInstance, windowSurface };
		Vulkan::LogicalDevice virtualGpu{ realGpu };

		//swapchain
		Vulkan::Swapchain swapchain{ realGpu, virtualGpu, windowSurface, window };


		//attachments for the render pass of the pipeline
		Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription colorAttachment{};
		Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription depthBufferAttachment{ Vulkan::PipelineOptions::RenderPassOptions::PredefinedAttachment::DEPTH };

		//attachments ready to be used in a render pass's subpass
		auto boundAttachments = Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription::prepareAttachments(colorAttachment, depthBufferAttachment);

		//subpasses
		Vulkan::PipelineOptions::RenderPassOptions::Subpass subpass(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, boundAttachments[0], boundAttachments[1]);
		//Vulkan::PipelineOptions::RenderPassOptions::Subpass s2(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, std::pair{ boundAttachments[2], true }, boundAttachments[0]);

		//render pass
		Vulkan::PipelineOptions::RenderPass renderPass{ virtualGpu, boundAttachments, subpass };

		//how a vertex is made up
		using MyVertex = Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3>;

		//vertices types descriptor
		Vulkan::PipelineOptions::PipelineVertexArrays vertexTypesDescriptor{ MyVertex{} };

		//uniform descriptors layouts
		Vulkan::DescriptorSetLayout globalLayout{ virtualGpu, std::tuple{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_ALL, 256}, std::tuple{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_ALL, 64} };
		Vulkan::DescriptorSetLayout perObjectLayout{ virtualGpu, std::tuple{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_ALL, int(16 * sizeof(float))}, std::tuple{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_ALL, int(7 * sizeof(float))} };
		
		//uniform buffers
		Vulkan::Buffers::UniformBuffer globalUniformBuffer{ virtualGpu, realGpu, 2048 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer perObjectUniformBuffer{ virtualGpu, realGpu, 1024 * sizeof(float) };
		globalUniformBuffer.fillBuffer(std::vector( 2048, 0.5f ));
		std::vector perObjectData1mvp{
			   1.0f, 0.0f, 0.0f, 0.0f,
			   0.0f, 1.0f, 0.0f, 0.0f,
			   0.0f, 0.0f, 1.0f, 0.0f,
			   0.5f, 0.5f, 0.0f, 1.0f
		};
		std::vector perObjectData1color{1.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f};


		std::vector perObjectData2mvp{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
		};

		std::vector perObjectData2color{ 0.0f, 1.0f, 0.0f, 0.0f, 0.9f, 0.1f, 0.9f};


		perObjectUniformBuffer.fillBuffer(perObjectData1mvp, perObjectData1color, perObjectData2mvp, perObjectData2color);

		//depth image view
		Vulkan::DepthImage depthBuffer{ virtualGpu, realGpu, swapchain.getResolution() };

		//pipeline options
		Vulkan::PipelineOptions::Multisampler multisampler{};
		Vulkan::PipelineOptions::DepthStencil depthStencil{};
		Vulkan::PipelineOptions::DynamicState dynamicState{};
		Vulkan::PipelineOptions::InputAssembly inputAssembly{};
		Vulkan::PipelineOptions::PipelineLayout pipelineLayout{virtualGpu, globalLayout, perObjectLayout};
		Vulkan::PipelineOptions::Rasterizer rasterizer{};
		Vulkan::PipelineOptions::Viewport viewport{};

		//shaders
		Vulkan::PipelineOptions::Shader vertexShader{ virtualGpu, "shaders/TestVert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT };
		Vulkan::PipelineOptions::Shader fragmentShader{ virtualGpu, "shaders/TestFrag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT };

		//pipeline
		Vulkan::Pipeline pipeline{ virtualGpu, renderPass, 0, std::vector{&vertexShader, &fragmentShader},vertexTypesDescriptor, pipelineLayout, inputAssembly, rasterizer, multisampler, depthStencil, dynamicState, viewport};
	
		//create drawer
		Vulkan::Drawer drawer{ virtualGpu, realGpu, window, windowSurface, swapchain, depthBuffer, renderPass, pipeline, globalUniformBuffer, perObjectUniformBuffer };

		//create models
		Vulkan::Model model1{ std::vector<MyVertex>{
			{{0.0f, -0.5f, 0.1f}, {1.0f, 0.0f, 0.0f}},
			{{-0.3f, 0.8f, 0.3f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.1f}, {0.0f, 0.0f, 1.0f}},
		},
		std::vector<uint32_t>{2, 1, 0}
		};
		Vulkan::Model model2{ std::vector<MyVertex>{
			{{0.5f, 0.5f,  0.2f}, {1.0f, 1.0f, 1.0f}},
			{{0.5f, 0.9f,  0.2f}, {1.0f, 1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.2f}, {1.0f, 1.0f, 1.0f}},
			{{-0.5f, 0.9f, 0.2f}, {1.0f, 1.0f, 1.0f}},
		},
		std::vector<uint32_t>{3, 2, 0, 0, 1, 3}
		};

		//create vertex buffer for models
		Vulkan::Buffers::VertexBuffer vertexBuffer{ virtualGpu, realGpu, (model1.getVertices().size() + model2.getVertices().size()) * sizeof(model1.getVertices()[0])};
		vertexBuffer.fillBuffer(model1, model2);

		//create index buffer for model
		Vulkan::Buffers::IndexBuffer indexBuffer{ virtualGpu, realGpu, (model1.getIndexes().size() + model2.getIndexes().size()) * sizeof(model1.getIndexes()[0]) };
		indexBuffer.fillBuffer(model1, model2);

		//draw cycle
		while (!glfwWindowShouldClose(+window)) {
			glfwPollEvents();
			drawer.draw(vertexBuffer, indexBuffer);
		}
		vkDeviceWaitIdle(+virtualGpu);

		std::cout << "\n";
	} catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}

	std::cout << "\n\n";
}
