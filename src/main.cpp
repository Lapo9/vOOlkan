#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Pinball.h"


template<typename Model>
void debugAnimation(Vulkan::Buffers::UniformBuffer& buffer, float delta, const Vulkan::DynamicSet& set, const std::vector<Model*>& models, std::chrono::nanoseconds elapsedNanoseconds);


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

		//command buffer pool
		Vulkan::CommandBufferPool commandBufferPool{ virtualGpu };

		//texture image
		Vulkan::TextureImage texture{ virtualGpu, realGpu, commandBufferPool, std::pair(512, 512), "textures/caTexture.jpg" };

		//depth image view
		Vulkan::DepthImage depthBuffer{ virtualGpu, realGpu, swapchain.getResolution() };


		//attachments for the render pass of the pipeline
		Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription colorAttachment{};
		Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription depthBufferAttachment{ Vulkan::PipelineOptions::RenderPassOptions::PredefinedAttachment::DEPTH };

		//attachments ready to be used in a render pass's subpass
		auto boundAttachments = Vulkan::PipelineOptions::RenderPassOptions::AttachmentDescription::prepareAttachments(colorAttachment, depthBufferAttachment);

		//subpasses
		Vulkan::PipelineOptions::RenderPassOptions::Subpass subpass(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, boundAttachments[0], boundAttachments[1]);

		//render pass
		Vulkan::PipelineOptions::RenderPass renderPass{ virtualGpu, boundAttachments, subpass };



		//how a vertex is made up
		using MyVertex = Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>;

		//vertices types descriptor
		Vulkan::PipelineOptions::PipelineVertexArrays vertexTypesDescriptor{ MyVertex{} };


		//uniform buffers
		Vulkan::Buffers::UniformBuffer globalUniformBuffer{ virtualGpu, realGpu, 2048 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer perObjectUniformBuffer{ virtualGpu, realGpu, 1024 * sizeof(float) };


		//uniform sets layouts
		using Lights = struct { glm::mat4 light1; };

		Vulkan::StaticSet globalSet{ virtualGpu, std::tuple{ VK_SHADER_STAGE_ALL, &texture}, std::tuple{ VK_SHADER_STAGE_ALL, Lights{}, &globalUniformBuffer, 0 } };
		Vulkan::DynamicSet perObjectSet{ realGpu, virtualGpu, perObjectUniformBuffer, std::pair{VK_SHADER_STAGE_ALL, glm::mat4{}}, std::pair{VK_SHADER_STAGE_ALL, glm::vec4{}} };
		


		//pipeline options
		Vulkan::PipelineOptions::Multisampler multisampler{};
		Vulkan::PipelineOptions::DepthStencil depthStencil{};
		Vulkan::PipelineOptions::DynamicState dynamicState{};
		Vulkan::PipelineOptions::InputAssembly inputAssembly{};
		Vulkan::PipelineOptions::PipelineLayout pipelineLayout{ virtualGpu, globalSet, perObjectSet };
		Vulkan::PipelineOptions::Rasterizer rasterizer{ false };
		Vulkan::PipelineOptions::Viewport viewport{};

		//shaders
		Vulkan::PipelineOptions::Shader vertexShader{ virtualGpu, "shaders/TestVert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT };
		Vulkan::PipelineOptions::Shader fragmentShader{ virtualGpu, "shaders/TestFrag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT };

		//pipeline
		Vulkan::Pipeline pipeline{ virtualGpu, renderPass, 0, std::vector{&vertexShader, &fragmentShader},vertexTypesDescriptor, pipelineLayout, rasterizer, inputAssembly, multisampler, depthStencil, dynamicState, viewport};
		



	
		//create drawer
		Vulkan::Drawer drawer{ virtualGpu, realGpu, window, windowSurface, swapchain, depthBuffer, commandBufferPool, renderPass, pipeline, globalSet, perObjectSet };

		//create models
		Vulkan::Model model1{ std::vector<MyVertex>{
			{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
			{{ 0.0f,  1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, -0.1f}},
			{{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		},
			std::vector<uint32_t>{0, 1, 2},
				glm::vec3{0.0f},
				glm::vec3{1.0f},
				glm::vec3{0.0f, 0.0f, -9.0f},
				glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}
		};

		Vulkan::Model model2{ std::vector<MyVertex>{
			{{-0.8f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{{-0.8f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.8f,   0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{{0.8f,  -0.5f,  0.0f}, {1.0f, 0.7f, 0.0f}, {1.0f, 1.0f}},
		},
			std::vector<uint32_t>{0, 1, 2, 0, 2, 3},
				glm::vec3{0.0f},
				glm::vec3{1.0f},
				glm::vec3{0.5f, 0.0f, -1.0f},
				glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}
		};

		Vulkan::Model model3{ std::vector<MyVertex>{
			{{-1.0f, -0.3f,  0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.5f}},
			{{-1.0f,  0.3f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{ 1.0f,  0.3f,  0.0f}, {0.5f, 0.0f, 1.0f}, {0.5f, 0.0f}},
			{{ 1.0f, -0.3f,  0.0f}, {1.0f, 0.0f, 0.5f}, {0.5f, 0.5f}},
		},
			std::vector<uint32_t>{0, 1, 2, 0, 2, 3},
				glm::vec3{0.0f},
				glm::vec3{1.0f},
				glm::vec3{-0.5f, 0.0f, -1.2f},
				glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}
		};



		//create vertex buffer for models
		Vulkan::Buffers::VertexBuffer vertexBuffer{ virtualGpu, realGpu, (model1.getVertices().size() + model2.getVertices().size() + model3.getVertices().size()) * sizeof(model1.getVertices()[0])};
		vertexBuffer.fillBuffer(model1 , model2, model3);

		//create index buffer for model
		Vulkan::Buffers::IndexBuffer indexBuffer{ virtualGpu, realGpu, (model1.getIndexes().size() + model2.getIndexes().size() + model3.getIndexes().size()) * sizeof(model1.getIndexes()[0]) };
		indexBuffer.fillBuffer(model1 , model2, model3);

		std::cout << "\n";

		//draw cycle
		auto lastFrameTime = std::chrono::high_resolution_clock::now();
		while (!glfwWindowShouldClose(+window)) {
			glfwPollEvents();
			debugAnimation(perObjectUniformBuffer, 0.1f, perObjectSet, std::vector{ &model1, &model2, &model3 }, std::chrono::high_resolution_clock::now() - lastFrameTime);
			lastFrameTime = std::chrono::high_resolution_clock::now();
			drawer.draw(vertexBuffer, indexBuffer);
		}
		vkDeviceWaitIdle(+virtualGpu);

		std::cout << "\n";
	} catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}

	std::cout << "\n\n";
}


template<typename Model>
void debugAnimation(Vulkan::Buffers::UniformBuffer& buffer, float delta, const Vulkan::DynamicSet& set, const std::vector<Model*>& models, std::chrono::nanoseconds elapsedNanoseconds){
	static float n = 0.1f, f = 9.9f, fovY = 120.0f, a = 1.0f;
	static  glm::mat4 perspective{
			1 / (a * glm::tan(glm::radians(fovY / 2))), 0, 0, 0,
			0, -1 / glm::tan(glm::radians(fovY / 2)), 0, 0,
			0, 0, f / (n - f), -1,
			0, 0, (n * f) / (n - f), 0
	};

	static glm::vec3 Angs{ 0.0f, 0.0f, 0.0f }, Pos{ 0.0f, 0.0f, 0.9f };
	static glm::mat4 view = glm::rotate(glm::mat4(1.0f), -Angs.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
		glm::rotate(glm::mat4(1.0f), -Angs.y, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), -Angs.x, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::mat4(1.0f), -Pos);

	float elapsedSeconds = elapsedNanoseconds.count() / 1000000000.0f;

	Model& model1 = *models[0], &model2 = *models[1], &model3 = *models[2];

	model1.rotate(0.8f * elapsedSeconds, glm::vec3{ 1.0f, 0.0f, 0.0f }).translate(glm::vec3{ 0.0f, 0.0f, 1.0f * elapsedSeconds });
	model2.rotate(1.0f * elapsedSeconds, glm::vec3{ 0.0f, 1.0f, 0.0f });
	model3.rotate(0.5f * elapsedSeconds, glm::vec3{ 0.0f, 1.0f, 0.0f });


	set.fillBuffer(buffer, model1.getUniforms(view, perspective), model2.getUniforms(view, perspective), model3.getUniforms(view, perspective));

}
