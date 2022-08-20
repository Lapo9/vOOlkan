#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Pinball.h"



void debugAnimation(Vulkan::Buffers::UniformBuffer& buffer, float delta);


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
		using MyVertex = Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3>;

		//vertices types descriptor
		Vulkan::PipelineOptions::PipelineVertexArrays vertexTypesDescriptor{ MyVertex{} };


		//uniform buffers
		Vulkan::Buffers::UniformBuffer globalUniformBuffer{ virtualGpu, realGpu, 2048 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer perObjectUniformBuffer{ virtualGpu, realGpu, 1024 * sizeof(float) };

		//uniform sets layouts
		using Lights = struct { glm::mat4 light1; };
		using Mvp = struct { glm::mat4 mvp; };
		using Color = struct { glm::vec4 rgb; };

		Vulkan::Set globalSet{ realGpu, virtualGpu, globalUniformBuffer, std::pair{Lights{}, VK_SHADER_STAGE_ALL} };
		Vulkan::Set perObjectSet{ realGpu, virtualGpu, perObjectUniformBuffer, std::pair{Mvp{}, VK_SHADER_STAGE_ALL}, std::pair{Color{}, VK_SHADER_STAGE_ALL} };
		


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
		Vulkan::Drawer drawer{ virtualGpu, realGpu, window, windowSurface, swapchain, depthBuffer, renderPass, pipeline, globalSet, perObjectSet };

		//create models
		Vulkan::Model model1{ std::vector<MyVertex>{
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.0f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		},
			std::vector<uint32_t>{0, 1, 2}
		};

		Vulkan::Model model2{ std::vector<MyVertex>{
			{{-0.8f, -0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}},
			{{-0.8f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}},
			{{0.8f,   0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}},
			{{0.8f,  -0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}},
		},
			std::vector<uint32_t>{0, 1, 2, 0, 2, 3}
		};

		Vulkan::Model model3{ std::vector<MyVertex>{
			{{-1.0f, -0.3f,  0.0f}, {0.0f, 0.0f, 0.0f}},
			{{-1.0f,  0.3f,  0.0f}, {1.0f, 0.0f, 0.0f}},
			{{ 1.0f,  0.3f,  0.0f}, {0.5f, 0.0f, 1.0f}},
			{{ 1.0f, -0.3f,  0.0f}, {1.0f, 0.0f, 0.5f}},
		},
			std::vector<uint32_t>{0, 1, 2, 0, 2, 3}
		};



		//create vertex buffer for models
		Vulkan::Buffers::VertexBuffer vertexBuffer{ virtualGpu, realGpu, (model1.getVertices().size() + model2.getVertices().size() + model3.getVertices().size()) * sizeof(model1.getVertices()[0])};
		vertexBuffer.fillBuffer(model1, model2, model3);

		//create index buffer for model
		Vulkan::Buffers::IndexBuffer indexBuffer{ virtualGpu, realGpu, (model1.getIndexes().size() + model2.getIndexes().size() + model3.getIndexes().size()) * sizeof(model1.getIndexes()[0]) };
		indexBuffer.fillBuffer(model1, model2, model3);

		std::cout << "\n";

		//draw cycle
		while (!glfwWindowShouldClose(+window)) {
			glfwPollEvents();
			debugAnimation(perObjectUniformBuffer, 0.1f);
			drawer.draw(vertexBuffer, indexBuffer);
		}
		vkDeviceWaitIdle(+virtualGpu);

		std::cout << "\n";
	} catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}

	std::cout << "\n\n";
}



void debugAnimation(Vulkan::Buffers::UniformBuffer& buffer, float delta){
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


	static glm::vec3 pos1 = glm::vec3(0.0f, 0.0f, -9.0f);
	static glm::vec3 pos2 = glm::vec3(-0.5f, -0.5f, -1.5f);
	static glm::vec3 pos3 = glm::vec3(0.2f, 0.0f, -1.5f);

	static float rotation1 = 0.0f;
	static float rotation2 = 0.0f;
	static float rotation3 = 0.0f;


	pos1 += glm::vec3(0.0f, 0.0f, 0.1f * delta);
	rotation1 += 0.4f * delta;
	rotation2 += 0.1f * delta;
	rotation3 += 0.3f * delta;


	glm::mat4 model1 = glm::translate(glm::mat4(1.0f), pos1) * glm::rotate(glm::mat4(1.0f), rotation1, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 model2 = glm::translate(glm::mat4(1.0f), pos2) * glm::rotate(glm::mat4(1.0f), rotation2, glm::vec3(0.0f, 1.0f, 0.0f));;
	glm::mat4 model3 = glm::translate(glm::mat4(1.0f), pos3) * glm::rotate(glm::mat4(1.0f), rotation3, glm::vec3(0.0f, 1.0f, 0.0f));;



	glm::mat4 mvp1 = perspective * view * model1;
	glm::mat4 mvp2 = perspective * view * model2;
	glm::mat4 mvp3 = perspective * view * model3;


	std::vector<float> perObjectData1mvp;
	std::vector<float> perObjectData2mvp;
	std::vector<float> perObjectData3mvp;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			perObjectData1mvp.push_back(mvp1[i][j]);
			perObjectData2mvp.push_back(mvp2[i][j]);
			perObjectData3mvp.push_back(mvp3[i][j]);
		}
	}



	std::vector perObjectData1color{ 1.0f, 0.0f, 0.0f, 0.0f};
	std::vector perObjectData2color{ 0.0f, 1.0f, 0.0f, 0.0f};
	std::vector perObjectData3color{ 0.0f, 0.0f, 1.0f, 0.0f};


	buffer.fillBuffer(perObjectData1mvp, perObjectData1color, perObjectData2mvp, perObjectData2color, perObjectData3mvp, perObjectData3color);
}
