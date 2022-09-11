#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Pinball.h"
#include "Animations.h"


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


template<typename... Models>
void debugAnimation(Vulkan::Buffers::UniformBuffer& mainPerObjectBuffer, const Vulkan::DynamicSet& mainPerObjectSet, Vulkan::Buffers::UniformBuffer& mainGlobalBuffer, const Vulkan::StaticSet& mainGlobalSet, Vulkan::Buffers::UniformBuffer& backgroundBuffer, const Vulkan::DynamicSet& backgroundSet, const std::tuple<Models*...>& models, Lights& lights, Vulkan::Physics::Universe& universe, Vulkan::Utilities::KeyboardListener& keyboardController, std::chrono::nanoseconds elapsedNanoseconds);

std::pair<std::vector<Vulkan::Physics::Hitbox*>, std::vector<Vulkan::Physics::Hitbox*>> getBallsStatus(std::vector<Vulkan::Physics::Hitbox*> balls) {
	//count how many balls are in play
	std::vector<Vulkan::Physics::Hitbox*> ballsInPlay{};
	std::vector<Vulkan::Physics::Hitbox*> ballsResting{};
	for (int i = 0; i < balls.size(); ++i) {
		if (balls[i]->getPosition().x() != RESTING_BALLS_POSITION.x()) {
			ballsInPlay.push_back(balls[i]);
		}
		else {
			ballsResting.push_back(balls[i]);
		}
	}
	return { ballsInPlay, ballsResting };
}


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
		using Matrices = struct {
			alignas(16) glm::mat4 mvp;
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 normals;
		};

		//fill global set 
		Lights lights{
			//point
			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 2.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{1.0f, 1.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{-1.0f, 1.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.7f, 3.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{-0.7f, 3.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.4f},

			glm::vec2{0.5f, 2.0f},

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
		Vulkan::Objects::Model ball1{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.162f, Vulkan::Physics::Position{0.9f, -3.0f, 0.0f}, 0.8f, 2.0f, Vulkan::Physics::Speed{0.0f, 0.0f, 0.0f}),
			{ 0.0_deg, 180.0_deg, 0.0_deg }, MyVertex{}, "models/ball.obj"
		};

		Vulkan::Objects::Model ball2{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.162f, RESTING_BALLS_POSITION, 0.8f, 2.0f, Vulkan::Physics::Speed{0.0f, 0.0f, 0.0f}),
			{ 0.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/ball.obj"
		};

		Vulkan::Objects::Model ball3{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.162f, RESTING_BALLS_POSITION, 0.8f, 2.0f, Vulkan::Physics::Speed{0.0f, 0.0f, 0.0f}),
			{ 0.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/ball.obj"
		};

		Vulkan::Objects::Model bumper1{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.376f, Vulkan::Physics::Position{lights.position0.x, lights.position0.y, 0.0f}, 0.8f),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/bumper.obj"
		};

		Vulkan::Objects::Model bumper2{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.376f, Vulkan::Physics::Position{lights.position1.x, lights.position1.y, 0.0f}, 0.8f),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/bumper.obj"
		};

		Vulkan::Objects::Model bumper3{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.376f, Vulkan::Physics::Position{lights.position2.x, lights.position2.y, 0.0f}, 0.8f),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/bumper.obj"
		};

		Vulkan::Objects::Model bumper4{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.376f, Vulkan::Physics::Position{lights.position3.x, lights.position3.y, 0.0f}, 0.8f),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/bumper.obj"
		};

		Vulkan::Objects::Model bumper5{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.376f, Vulkan::Physics::Position{lights.position4.x, lights.position4.y, 0.0f}, 0.8f),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/bumper.obj"
		};


		Vulkan::Objects::Model rightFlipper{ std::make_unique<Vulkan::Physics::FrameHitbox>(Vulkan::Physics::Position{1.35f, -4.7f, 0.0f}, 0.9f, Vulkan::Physics::Position{0.5f, 0.15f, 0.0f}, Vulkan::Physics::Position{-0.8f, 0.05f, 0.0f}),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/flipper.obj" 
		};
		rightFlipper.setKeyPressResponse(Vulkan::Animations::rightPadUp<MyVertex>);

		Vulkan::Objects::Model leftFlipper{ std::make_unique<Vulkan::Physics::FrameHitbox>(Vulkan::Physics::Position{-0.8f, -4.7f, 0.0f}, 0.9f, Vulkan::Physics::Position{0.5f, -0.15f, 0.0f}, Vulkan::Physics::Position{-0.8f, -0.05f, 0.0f}),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/flipper.obj"
		};
		(+leftFlipper).rotate(180.0_deg, { 0.0f, 0.0f, 1.0f });
		leftFlipper.setKeyPressResponse(Vulkan::Animations::leftPadUp<MyVertex>);

		Vulkan::Objects::Model body{ std::make_unique<Vulkan::Physics::FrameHitbox>(Vulkan::Physics::Position{0.0f, 0.0f, 0.0f}, 1.0f, 
			Vulkan::Physics::Position{-0.949f, -5.950f, 0.0f}, 
			Vulkan::Physics::Position{-0.949f, -4.127f, 0.0f}, 
			Vulkan::Physics::Position{-2.386f, -3.277f, 0.0f},
			Vulkan::Physics::Position{-2.386f,  4.076f, 0.0f}, 
			Vulkan::Physics::Position{-0.640f,  5.447f, 0.0f}, 
			Vulkan::Physics::Position{ 0.877f,  5.447f, 0.0f}, 
			Vulkan::Physics::Position{ 2.700f,  4.076f, 0.0f}, 
			Vulkan::Physics::Position{ 2.700f, -3.325f, 0.0f}, 
			Vulkan::Physics::Position{ 1.525f, -4.127f, 0.0f}, 
			Vulkan::Physics::Position{ 1.525f, -5.950f, 0.0f}, 
			Vulkan::Physics::Position{-0.949f, -5.95f, 0.0f}),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/body.obj"
		};


		Vulkan::Objects::Model redLight{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.5f, lights.position0, 0.01f),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/square.obj",
			lights.color0 
		};

		Vulkan::Objects::Model greenLight{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.5f, lights.position1, 0.01f),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/square.obj",
			lights.color1
		};

		Vulkan::Objects::Model blueLight{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.5f, lights.position2, 0.01f),
			{ 90.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/square.obj",
			lights.color2
		};


		Vulkan::Physics::FrameHitbox ballKiller{ Vulkan::Physics::Position{0.0f, -5.6f, 0.0f}, 1.0f, Vulkan::Physics::Position{-2.0f, 0.0f,0.0f}, Vulkan::Physics::Position{2.0f, 0.0f,0.0f} };


		Vulkan::Physics::Field gravity{ Vulkan::Physics::Position{1.0f, 0.0f, -2.0f}, Vulkan::Physics::FieldFunctions::gravity<60> };
		Vulkan::Physics::Field friction{ Vulkan::Physics::Position{0.0f, 0.0f, -2.0f}, Vulkan::Physics::FieldFunctions::friction<3.0> };

		
		//add models to universe
		Vulkan::Physics::Universe physicsUniverse{ std::vector<Vulkan::Physics::Field*>{&gravity, &friction}, +ball1, +bumper1, +bumper2, +bumper3, +bumper4, +bumper5, +rightFlipper, +leftFlipper, +body, ballKiller};

		//add keyboard press controller
		Vulkan::Utilities::KeyboardListener keyboardController{ window, rightFlipper, leftFlipper };

		//collision actions
		std::vector<Vulkan::Physics::Hitbox*> balls{ &(+ball1), &(+ball2), &(+ball3) };
		(+bumper1).setCollisionAction([&balls, &physicsUniverse, &lights](Vulkan::Physics::Hitbox&) {
			if (lights.color0 == glm::vec3{ 0.0f, 0.0f, 0.0f }) {
				lights.color0 = { 1.0f, 0.0f, 1.0f };
			}
			else {
				lights.color0 = { 0.0f, 0.0f, 0.0f };
			}
			auto [activeBalls, restingBalls] = getBallsStatus(balls);
			if (activeBalls.size() == 1) {
				for (int i = 0; i < restingBalls.size(); ++i) {
					restingBalls[i]->setPosition({ i * 4.0f - 2.0f, 0.0f, 0.0f });
					physicsUniverse.addBody(*restingBalls[i]);
				}
			}
			});

		(+bumper2).setCollisionAction([&balls, &physicsUniverse, &lights](Vulkan::Physics::Hitbox&) {
			if (lights.color1 == glm::vec3{ 0.0f, 0.0f, 0.0f }) {
				lights.color1 = { 1.0f, 0.0f, 0.0f };
				}
			else {
				lights.color1 = { 0.0f, 0.0f, 0.0f };
			}
			});

		(+bumper3).setCollisionAction([&balls, &physicsUniverse, &lights](Vulkan::Physics::Hitbox&) {
			if (lights.color2 == glm::vec3{ 0.0f, 0.0f, 0.0f }) {
				lights.color2 = { 1.0f, 0.75f, 0.0f };
			}
			else {
				lights.color2 = { 0.0f, 0.0f, 0.0f };
			}
			});

		(+bumper4).setCollisionAction([&balls, &physicsUniverse, &lights](Vulkan::Physics::Hitbox&) {
			if (lights.color3 == glm::vec3{ 0.0f, 0.0f, 0.0f }) {
				lights.color3 = { 0.0f, 1.0f, 0.0f };
			}
			else {
				lights.color3 = { 0.0f, 0.0f, 0.0f };
			}
			});

		(+bumper5).setCollisionAction([&balls, &physicsUniverse, &lights](Vulkan::Physics::Hitbox&) {
			if (lights.color4 == glm::vec3{ 0.0f, 0.0f, 0.0f }) {
				lights.color4 = { 0.0f, 0.0f, 1.0f };
			}
			else {
				lights.color4 = { 0.0f, 0.0f, 0.0f };
			}
			});

		
		ballKiller.setCollisionAction([&balls, &physicsUniverse](Vulkan::Physics::Hitbox& collidingBall) {
			physicsUniverse.removeBody(collidingBall);
			collidingBall.reset(RESTING_BALLS_POSITION);
			});



		// ================ VERTEX/INDEX BUFFERS SETUP ================

		//vertex buffers
		Vulkan::Buffers::VertexBuffer mainVertexBuffer{ virtualGpu, realGpu, (ball1.getVertices().size() + ball2.getVertices().size() + ball3.getVertices().size() + bumper1.getVertices().size() + bumper2.getVertices().size() + bumper3.getVertices().size() + bumper4.getVertices().size() + bumper5.getVertices().size() + rightFlipper.getVertices().size() + leftFlipper.getVertices().size() + body.getVertices().size()) * sizeof(MyVertex)};
		Vulkan::Buffers::VertexBuffer backgroundVertexBuffer{ virtualGpu, realGpu, (redLight.getVertices().size() + greenLight.getVertices().size() + blueLight.getVertices().size()) * sizeof(MyVertex) };
		mainVertexBuffer.fillBuffer(ball1, ball2, ball3, bumper1, bumper2, bumper3, bumper4, bumper5, rightFlipper, leftFlipper, body);
		backgroundVertexBuffer.fillBuffer(redLight, greenLight, blueLight);

		//index buffers
		Vulkan::Buffers::IndexBuffer mainIndexBuffer{ virtualGpu, realGpu, (ball1.getIndexes().size() + ball2.getIndexes().size() + ball3.getIndexes().size() + bumper1.getIndexes().size() + bumper2.getIndexes().size() + bumper3.getIndexes().size() + bumper4.getIndexes().size() + bumper5.getIndexes().size() + rightFlipper.getIndexes().size() + leftFlipper.getIndexes().size() + body.getVertices().size()) * sizeof(uint32_t)};
		Vulkan::Buffers::IndexBuffer backgroundIndexBuffer{ virtualGpu, realGpu, (redLight.getIndexes().size() + greenLight.getIndexes().size() + blueLight.getIndexes().size()) * sizeof(uint32_t) };
		mainIndexBuffer.fillBuffer(ball1, ball2, ball3, bumper1, bumper2, bumper3, bumper4, bumper5, rightFlipper, leftFlipper, body);
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
			debugAnimation(mainPerObjectUniformBuffer, mainPerObjectSet, mainGlobalUniformBuffer, mainGlobalSet, backgroundPerObjectUniformBuffer, backgroundPerObjectSet, std::tuple{ &ball1, &ball2, &ball3, &bumper1, &bumper2, &bumper3, &bumper4, &bumper5, &rightFlipper, &leftFlipper, &body, &redLight, &greenLight, &blueLight }, lights, physicsUniverse, keyboardController, std::chrono::high_resolution_clock::now() - lastFrameTime);
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
	return Vulkan::Physics::Force{ (fieldPos - objPos) * 1.0f };
}


template<typename... Models>
void debugAnimation(Vulkan::Buffers::UniformBuffer& mainPerObjectBuffer, const Vulkan::DynamicSet& mainPerObjectSet, Vulkan::Buffers::UniformBuffer& mainGlobalBuffer, const Vulkan::StaticSet& mainGlobalSet, Vulkan::Buffers::UniformBuffer& backgroundBuffer, const Vulkan::DynamicSet& backgroundSet, const std::tuple<Models*...>& models, Lights& lights, Vulkan::Physics::Universe& universe, Vulkan::Utilities::KeyboardListener& keyboardController, std::chrono::nanoseconds elapsedNanoseconds){
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

	static auto camera = Vulkan::Objects::Camera{ {0.0f, -4.0f, 5.0f}, {0.0_deg, 60.0_deg, 0.0_deg} };
	//static auto camera = Vulkan::Objects::Camera{ {0.0f, 0.0f, 5.0f}, {0.0_deg, 0.0_deg, 0.0_deg} };
	camera.rotate(0.0f * elapsedSeconds, { 0.0f, 0.0f, 1.0f });

	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& ball1 = *std::get<0>(models); 
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& ball2 = *std::get<1>(models); 
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& ball3 = *std::get<2>(models); 
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& bumper1 = *std::get<3>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& bumper2 = *std::get<4>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& bumper3 = *std::get<5>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& bumper4 = *std::get<6>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& bumper5 = *std::get<7>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& rightFlipper = *std::get<8>(models); 
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& leftFlipper = *std::get<9>(models); 
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& body = *std::get<10>(models);
	auto& redLight = *std::get<11>(models); auto& greenLight = *std::get<12>(models); auto& blueLight = *std::get<13>(models);

	//model1.rotate(0.57f * elapsedSeconds, glm::vec3{ 0.0f, 1.0f, 0.0f }).rotate(0.37f * elapsedSeconds, glm::vec3{ 1.0f, 0.0f, 0.0f });
	//model2.rotate(1.1f * elapsedSeconds, glm::vec3{ 0.0f, 1.0f, 0.0f });
	//model3.rotate(0.49f * elapsedSeconds, glm::vec3{ 0.0f, 1.0f, 0.0f });

	//model1.move(elapsedSeconds, {0.0f, 0.0f, model1.getPosition().z() < -3.0f ? 1.0f : -1.0f});

	//(+model1)->move(elapsedSeconds);

	(+rightFlipper).setAngularSpeed(0.0f);
	if ((+rightFlipper).getRotationEuler()[0] < -FLIPPER_MIN_ANGLE) {
		(+rightFlipper).setAngularSpeed(FLIPPER_ANGULAR_SPEED);
	}
	(+leftFlipper).setAngularSpeed(0.0f);
	if ((+leftFlipper).getRotationEuler()[0] > 180.0_deg + FLIPPER_MIN_ANGLE || (+leftFlipper).getRotationEuler()[0] < 0.0_deg) {
		(+leftFlipper).setAngularSpeed(-FLIPPER_ANGULAR_SPEED);
	}
	keyboardController.checkKeyPressed();

	universe.calculate(elapsedSeconds);

	glm::mat4 projection = perspective;

	mainPerObjectSet.fillBuffer(mainPerObjectBuffer, 
		ball1.getUniforms(camera.getViewMatrix(), projection), 
		ball2.getUniforms(camera.getViewMatrix(), projection), 
		ball3.getUniforms(camera.getViewMatrix(), projection), 
		bumper1.getUniforms(camera.getViewMatrix(), projection),
		bumper2.getUniforms(camera.getViewMatrix(), projection),
		bumper3.getUniforms(camera.getViewMatrix(), projection),
		bumper4.getUniforms(camera.getViewMatrix(), projection),
		bumper5.getUniforms(camera.getViewMatrix(), projection),
		rightFlipper.getUniforms(camera.getViewMatrix(), projection), 
		leftFlipper.getUniforms(camera.getViewMatrix(), projection), 
		body.getUniforms(camera.getViewMatrix(), projection));

	backgroundSet.fillBuffer(backgroundBuffer, redLight.getUniforms(camera.getViewMatrix(), projection), greenLight.getUniforms(camera.getViewMatrix(), projection), blueLight.getUniforms(camera.getViewMatrix(), projection));

	mainGlobalSet.fillBuffer(mainGlobalBuffer, lights);
}





