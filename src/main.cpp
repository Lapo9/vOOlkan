#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Pinball.h"


template<typename... Models>
void debugAnimation(Vulkan::Buffers::UniformBuffer& mainBuffer, Vulkan::Buffers::UniformBuffer& backgroundBuffer, const Vulkan::DynamicSet& mainSet, const Vulkan::DynamicSet& backgroundSet, const std::tuple<Models*...>& models, std::chrono::nanoseconds elapsedNanoseconds);


int main() {
	try {

		// ================ GPU AND SWAPCHAIN SETUP ================
		//GPU setup
		Vulkan::Window window{ 1000, 1000, "Test app title" };
		Vulkan::Instance vulkanInstance{ "test app" };
		Vulkan::WindowSurface windowSurface{ vulkanInstance, window };
		Vulkan::PhysicalDevice realGpu{ vulkanInstance, windowSurface };
		Vulkan::LogicalDevice virtualGpu{ realGpu };

		//swapchain
		Vulkan::Swapchain swapchain{ realGpu, virtualGpu, windowSurface, window };

		//command buffer pool
		Vulkan::CommandBufferPool commandBufferPool{ virtualGpu };

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






		// ================ MODELS SETUP ================

		//how a vertex is made
		using MyVertex = Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>;

		//uniform sets layouts
		using Lights = struct {
			alignas(16) glm::vec3 color0;
			alignas(16) glm::vec3 position0;
			alignas(16) glm::vec3 color1;
			alignas(16) glm::vec3 position1;
			alignas(16) glm::vec3 color2;
			alignas(16) glm::vec3 position2;
			alignas(16) glm::vec3 color3;
			alignas(16) glm::vec3 position3;
			alignas(16) glm::vec3 color4;
			alignas(16) glm::vec3 position4;
			alignas(16) glm::vec3 color5;
			alignas(16) glm::vec3 position5;
			alignas(8) glm::vec2 decayFactor;

			alignas(16) glm::vec3 directionalLightColor;
			alignas(16) glm::vec3 directionalLightDirection;

			alignas(16) glm::vec3 basicAmbient;
			alignas(16) glm::vec3 dxColor;
			alignas(16) glm::vec3 dyColor;
			alignas(16) glm::vec3 dzColor;

			alignas(16) glm::vec3 eyePosition;
		};
		using Matrices = struct {
			alignas(16) glm::mat4 mvp;
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 normals;
		};

		//fill global set 
		Lights lights{
			//point
			glm::vec3{1.0f, 0.0f, 0.0f},
			glm::vec3{1.0f, -0.3f, -1.0f},
			glm::vec3{0.0f, 1.0f, 0.0f},
			glm::vec3{-1.0f, -0.3f, -2.0f},
			glm::vec3{0.0f, 0.0f, 1.0f},
			glm::vec3{0.0f, 1.0f, -3.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec2{3.0f, 2.0f},

			//dir
			glm::vec3{0.2f, 0.2f, 0.2f},
			glm::vec3{0.0f, 0.0f, 1.0f},

			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},

			//eye
			glm::vec3{0.0f, 0.0f, 0.0f}
		};

		//create models
		Vulkan::Objects::Model model1{ MyVertex{}, "models/pinball.obj",
			glm::vec3{0.0_deg, 180.0_deg, 0.0_deg},
			glm::vec3{0.1f},
			glm::vec3{0.0f, -1.3f, -0.7f}
		};


		Vulkan::Objects::Model floor{ MyVertex{}, "models/square.obj",
			glm::vec3{0.0_deg, 0.0_deg, 0.0_deg},
			glm::vec3{2.0f},
			glm::vec3{0.0f, -2.0f, -200.0f}
		};

		Vulkan::Objects::Model redLight{ MyVertex{}, "models/square.obj",
			glm::vec3{90.0_deg, 0.0_deg, 0.0_deg},
			glm::vec3{0.1f},
			lights.position0,
			lights.color0
			
		};

		Vulkan::Objects::Model greenLight{ MyVertex{}, "models/square.obj",
			glm::vec3{90.0_deg, 0.0_deg, 0.0_deg},
			glm::vec3{0.1f},
			lights.position1,
			lights.color1
		};
				
		Vulkan::Objects::Model blueLight{ MyVertex{}, "models/square.obj",
			glm::vec3{90.0_deg, 0.0_deg, 0.0_deg},
			glm::vec3{0.1f},
			lights.position2,
			lights.color2
		};





		// ================ VERTEX/INDEX BUFFERS SETUP ================

		//vertex buffers
		Vulkan::Buffers::VertexBuffer mainVertexBuffer{ virtualGpu, realGpu, (model1.getVertices().size() + floor.getVertices().size()) * sizeof(MyVertex) };
		Vulkan::Buffers::VertexBuffer backgroundVertexBuffer{ virtualGpu, realGpu, (redLight.getVertices().size() + greenLight.getVertices().size() + blueLight.getVertices().size()) * sizeof(MyVertex) };
		mainVertexBuffer.fillBuffer(model1, floor);
		backgroundVertexBuffer.fillBuffer(redLight, greenLight, blueLight);

		//index buffers
		Vulkan::Buffers::IndexBuffer mainIndexBuffer{ virtualGpu, realGpu, (model1.getIndexes().size() + floor.getIndexes().size()) * sizeof(uint32_t) };
		Vulkan::Buffers::IndexBuffer backgroundIndexBuffer{ virtualGpu, realGpu, (redLight.getIndexes().size() + greenLight.getIndexes().size() + blueLight.getIndexes().size()) * sizeof(uint32_t) };
		mainIndexBuffer.fillBuffer(model1, floor);
		backgroundIndexBuffer.fillBuffer(redLight, greenLight, blueLight);






		// ================ UNIFORMS/TEXTURES SETUP ================

		//texture image
		Vulkan::TextureImage mainTexture{ virtualGpu, realGpu, commandBufferPool, std::pair(1024, 1024), "textures/white.jpg" };
		Vulkan::TextureImage backgroundTexture{ virtualGpu, realGpu, commandBufferPool, std::pair(512, 512), "textures/light.png" };

		//uniform buffers
		Vulkan::Buffers::UniformBuffer mainGlobalUniformBuffer{ virtualGpu, realGpu, 2048 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer mainPerObjectUniformBuffer{ virtualGpu, realGpu, 1024 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer backgroundGlobalUniformBuffer{ virtualGpu, realGpu, 2048 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer backgroundPerObjectUniformBuffer{ virtualGpu, realGpu, 1024 * sizeof(float) };

		//descriptor sets
		Vulkan::StaticSet mainGlobalSet{ virtualGpu, std::tuple{ VK_SHADER_STAGE_ALL, &mainTexture}, std::tuple{ VK_SHADER_STAGE_ALL, Lights{}, &mainGlobalUniformBuffer, 0 } };
		Vulkan::DynamicSet mainPerObjectSet{ realGpu, virtualGpu, mainPerObjectUniformBuffer, std::pair{VK_SHADER_STAGE_ALL, Matrices{}} };
		Vulkan::StaticSet backgroundGlobalSet{ virtualGpu, std::tuple{VK_SHADER_STAGE_ALL, &backgroundTexture} };
		Vulkan::DynamicSet backgroundPerObjectSet{ realGpu, virtualGpu, backgroundPerObjectUniformBuffer, std::pair{VK_SHADER_STAGE_ALL, Matrices{}}, std::pair{VK_SHADER_STAGE_ALL, glm::vec3{}} };






		// ================ PIPELINE SETUP ================

		//common pipeline options
		Vulkan::PipelineOptions::PipelineVertexArrays vertexTypesDescriptor{ MyVertex{} }; //type of the vertices
		Vulkan::PipelineOptions::Multisampler multisampler{};
		Vulkan::PipelineOptions::DepthStencil depthStencil{};
		Vulkan::PipelineOptions::DynamicState dynamicState{};
		Vulkan::PipelineOptions::InputAssembly inputAssembly{};
		Vulkan::PipelineOptions::Rasterizer rasterizer{ false };
		Vulkan::PipelineOptions::Viewport viewport{};

		//main pipeline
		Vulkan::PipelineOptions::PipelineLayout mainPipelineLayout{ virtualGpu, mainGlobalSet, mainPerObjectSet };
		Vulkan::PipelineOptions::Shader mainVertexShader{ virtualGpu, "shaders/VertexShaderVert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT };
		Vulkan::PipelineOptions::Shader mainFragmentShader{ virtualGpu, "shaders/FragmentShaderFrag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT };

		Vulkan::Pipeline mainPipeline{ virtualGpu, renderPass, 0, std::vector{&mainVertexShader, &mainFragmentShader},vertexTypesDescriptor, mainPipelineLayout, rasterizer, inputAssembly, multisampler, depthStencil, dynamicState, viewport };

		//background pipeline
		Vulkan::PipelineOptions::PipelineLayout backgroundPipelineLayout{ virtualGpu, backgroundGlobalSet, backgroundPerObjectSet };
		Vulkan::PipelineOptions::Shader backgroundVertexShader{ virtualGpu, "shaders/TestVert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT };
		Vulkan::PipelineOptions::Shader backgroundFragmentShader{ virtualGpu, "shaders/TestFrag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT };

		Vulkan::Pipeline backgroundPipeline{ virtualGpu, renderPass, 0, std::vector{&backgroundVertexShader, &backgroundFragmentShader},vertexTypesDescriptor, backgroundPipelineLayout, rasterizer, inputAssembly, multisampler, depthStencil, dynamicState, viewport };



		


		mainGlobalSet.fillBuffer(mainGlobalUniformBuffer, lights);



	
		//create drawer
		Vulkan::Drawer drawer{ virtualGpu, realGpu, window, windowSurface, swapchain, depthBuffer, commandBufferPool, renderPass, 
			{&mainPipeline, &backgroundPipeline},
			{mainGlobalSet, backgroundGlobalSet},
			{mainPerObjectSet, backgroundPerObjectSet} };



		std::cout << "\n";

		//draw cycle
		auto lastFrameTime = std::chrono::high_resolution_clock::now();
		while (!glfwWindowShouldClose(+window)) {
			glfwPollEvents();
			debugAnimation(mainPerObjectUniformBuffer, backgroundPerObjectUniformBuffer, mainPerObjectSet, backgroundPerObjectSet, std::tuple{ &model1, &floor, &redLight, &greenLight, &blueLight }, std::chrono::high_resolution_clock::now() - lastFrameTime);
			lastFrameTime = std::chrono::high_resolution_clock::now();
			drawer.draw(
				std::pair< std::reference_wrapper<Vulkan::Buffers::VertexBuffer>, std::reference_wrapper<Vulkan::Buffers::IndexBuffer>>{ mainVertexBuffer, mainIndexBuffer },
				std::pair< std::reference_wrapper<Vulkan::Buffers::VertexBuffer>, std::reference_wrapper<Vulkan::Buffers::IndexBuffer>>{ backgroundVertexBuffer, backgroundIndexBuffer }
			);
		}
		vkDeviceWaitIdle(+virtualGpu);

		std::cout << "\n";
	} catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}

	std::cout << "\n\n";
}




Vulkan::Physics::Force foo(Vulkan::Physics::Position fieldPos, Vulkan::Physics::Position objPos) {
	return Vulkan::Physics::Force{ glm::vec3(fieldPos - objPos) * 1.0f };
}


template<typename... Models>
void debugAnimation(Vulkan::Buffers::UniformBuffer& mainBuffer, Vulkan::Buffers::UniformBuffer& backgroundBuffer, const Vulkan::DynamicSet& mainSet, const Vulkan::DynamicSet& backgroundSet, const std::tuple<Models*...>& models, std::chrono::nanoseconds elapsedNanoseconds){
	static float n = 0.1f, f = 9.9f, fovY = 120.0f, a = 1.0f, w = 1.0f;
	static  glm::mat4 perspective{
			1 / (a * glm::tan(glm::radians(fovY / 2))), 0, 0, 0,
			0, -1 / glm::tan(glm::radians(fovY / 2)), 0, 0,
			0, 0, f / (n - f), -1,
			0, 0, (n * f) / (n - f), 0
	};

	static glm::mat4 parallel{
		1 / w, 0, 0, 0,
		0, -a / w, 0, 0,
		0, 0, 1 / (n - f), 0,
		0, 0, n / (n - f), 1
	};

	float elapsedSeconds = elapsedNanoseconds.count() / 1000000000.0f;

	static auto camera = Vulkan::Objects::Camera{ {0.0f, 0.0f, 0.0f}, {0.0_deg, -45.0_deg, 0.0_deg} };
	camera.rotate(0.0f * elapsedSeconds, { 1.0f, 0.0f, 0.0f });

	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& model1 = *std::get<0>(models); auto& floor = *std::get<1>(models);
	auto& redLight = *std::get<2>(models); auto& greenLight = *std::get<3>(models); auto& blueLight = *std::get<4>(models);

	//model1.rotate(0.57f * elapsedSeconds, glm::vec3{ 0.0f, 1.0f, 0.0f }).rotate(0.37f * elapsedSeconds, glm::vec3{ 1.0f, 0.0f, 0.0f });
	//model2.rotate(1.1f * elapsedSeconds, glm::vec3{ 0.0f, 1.0f, 0.0f });
	//model3.rotate(0.49f * elapsedSeconds, glm::vec3{ 0.0f, 1.0f, 0.0f });

	//model1.move(elapsedSeconds, {0.0f, 0.0f, model1.getPosition().z() < -3.0f ? 1.0f : -1.0f});

	static Vulkan::Physics::Field field{ {0.0f, 0.0f, -3.0f}, foo };
	field.getCinematicable().move(elapsedSeconds, { 0.1f, 0.0f, 0.0f });

	model1.move(elapsedSeconds, field.calculateAppliedForce(model1.getPosition()));

	glm::mat4 projection = perspective;

	mainSet.fillBuffer(mainBuffer, model1.getUniforms(camera.getViewMatrix(), projection), floor.getUniforms(camera.getViewMatrix(), projection));
	backgroundSet.fillBuffer(backgroundBuffer, redLight.getUniforms(camera.getViewMatrix(), projection), greenLight.getUniforms(camera.getViewMatrix(), projection), blueLight.getUniforms(camera.getViewMatrix(), projection));

}
