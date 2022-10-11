#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Pinball.h"
#include "Animations.h"
#include "GameStatus.h"



template<typename... Models>
void calculateGraphics(Vulkan::Buffers::UniformBuffer& mainPerObjectBuffer, const Vulkan::DynamicSet& mainPerObjectSet, Vulkan::Buffers::UniformBuffer& mainGlobalBuffer, const Vulkan::StaticSet& mainGlobalSet, Vulkan::Buffers::UniformBuffer& backgroundBuffer, const Vulkan::DynamicSet& backgroundSet, Vulkan::Buffers::VertexBuffer& backgroundVertexBuffer, const std::tuple<Models*...>& models, Lights& lights, Vulkan::Physics::Universe& universe, Vulkan::Physics::Universe& pullerUniverse, Vulkan::Utilities::KeyboardListener& keyboardController, float aspectRatio, int points);


void calculatePhysics(std::vector<Vulkan::Physics::Universe*> universes, Vulkan::Utilities::KeyboardListener& kc, Vulkan::Physics::Hitbox& leftFlipper, Vulkan::Physics::Hitbox& rightFlipper, std::chrono::nanoseconds elapsedNanoseconds);


std::vector<MyVertex> buildPointDisplayerVertices(int digit) {
	const float xOffset = 0.001f; const float yOffset = 0.0f;
	const float width = 0.07f; const float height = 1.0f/10.0f;

	std::vector<MyVertex> pointDisplayerVertices{
	{{-0.5f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {xOffset 		, yOffset + height * digit + height }},
	{{-0.5f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {xOffset 		, yOffset + height * digit			}},
	{{ 0.5f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {xOffset + width , yOffset + height * digit + height }},
	{{ 0.5f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {xOffset + width , yOffset + height * digit 			}}
	};

	return pointDisplayerVertices;
}



int main() {
	try {

		// ================ GPU AND SWAPCHAIN SETUP ================
		//GPU setup
		Vulkan::Window window{ 1000, 1000, "Pinball" };
		Vulkan::Instance vulkanInstance{ "Pinball" };
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

		using Matrices = struct {
			alignas(16) glm::mat4 mvp;
			alignas(16) glm::mat4 model;
			alignas(16) glm::mat4 normals;
		};

		//fill global set 
		Lights lights{
			//point (color, pos)
			glm::vec3{0.0f, 0.0f, 0.0f}, //bumper1
			glm::vec3{0.0f, 2.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f}, //bumper2
			glm::vec3{1.0f, 1.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f}, //bumper3
			glm::vec3{-1.0f, 1.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f}, //bumper4
			glm::vec3{0.7f, 3.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f}, //bumper5
			glm::vec3{-0.7f, 3.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f}, //ball1
			glm::vec3{0.0f, 0.0f, 0.4f},

			//decay factors
			glm::vec2{0.5f, 2.0f},
			glm::vec2{1.0f, 2.0f},
			
			//points (again) (color, pos)
			glm::vec3{0.0f, 0.0f, 0.0f}, //ball2
			glm::vec3{0.0f, 0.0f, 0.0f},

			glm::vec3{0.0f, 0.0f, 0.0f}, //ball3
			glm::vec3{0.0f, 0.0f, 0.4f},

			glm::vec3{0.0f, 0.0f, 0.0f},
			glm::vec3{0.0f, 0.0f, 0.0f},

			//dir
			glm::vec3{1.0f, 1.0f, 1.0f},
			//glm::vec3{0.0f, -1.0f, 0.7f},
			glm::vec3{0.0f, -1.0f, 0.7f},

			//glm::vec3{0.87891f, 0.87891f, 0.87891f}, -> This is a bit too light up!
			glm::vec3{0.44141f, 0.44141f, 0.44141f},
			glm::vec3{0.89453f, 0.83984f, 0.64844f}, // This is a light brown for the base of the pinball
			glm::vec3{0.48828f, 0.23047f, 0.93750f}, // This is a blue for the sky
			glm::vec3{0.0f, -1.0f, 0.0f},

			//eye
			glm::vec3{0.0f, 0.0f, 2.0f},

			//shaders to use
			glm::vec2{0, 2}
		};

		//create models
		Vulkan::Objects::Model ball1{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.162f, RESTING_BALLS_POSITION, 0.8f, 2.0f),
			{ 0.0_deg, 180.0_deg, 0.0_deg }, MyVertex{}, "models/ball.obj"
		};

		Vulkan::Objects::Model ball2{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.162f, RESTING_BALLS_POSITION, 0.8f, 2.0f),
			{ 0.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/ball.obj"
		};

		Vulkan::Objects::Model ball3{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.162f, RESTING_BALLS_POSITION, 0.8f, 2.0f),
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


		Vulkan::Objects::Model puller{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.05f, PULLER_RESTING_POSITION, 1.0f, 1.0f),
			{ 180.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/puller.obj"
		};
		puller.setKeyPressResponse(Vulkan::Animations::pullerDown<MyVertex>);


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


		Vulkan::Objects::Model skybox{ std::make_unique<Vulkan::Physics::CircleHitbox>(0.5f, Vulkan::Physics::Position{0.0f, -4.2f, 5.5f}, 1000.0f),
			{ 0.0_deg, 0.0_deg, 0.0_deg }, MyVertex{}, "models/cube.obj"
		};


		Vulkan::Objects::Model point1{ {95.0_deg, 0.0_deg, 0.0_deg}, 0.7f, {2.0f, 5.89f, 4.0f}, buildPointDisplayerVertices(0), std::vector<uint32_t>{0, 2, 1, 2, 3, 1} };
		Vulkan::Objects::Model point10{ {95.0_deg, 0.0_deg, 0.0_deg}, 0.7f, {0.6f, 5.89f, 4.0f}, buildPointDisplayerVertices(0), std::vector<uint32_t>{0, 2, 1, 2, 3, 1} };
		Vulkan::Objects::Model point100{ {95.0_deg, 0.0_deg, 0.0_deg}, 0.7f, {-0.6f, 5.89f, 4.0f}, buildPointDisplayerVertices(0), std::vector<uint32_t>{0, 2, 1, 2, 3, 1} };
		Vulkan::Objects::Model point1000{ {95.0_deg, 0.0_deg, 0.0_deg}, 0.7f, {-2.0f, 5.89f, 4.0f}, buildPointDisplayerVertices(0), std::vector<uint32_t>{0, 2, 1, 2, 3, 1} };



		Vulkan::Physics::FrameHitbox ballKiller{ Vulkan::Physics::Position{0.0f, -5.6f, 0.0f}, 1.0f, Vulkan::Physics::Position{-2.0f, 0.0f,0.0f}, Vulkan::Physics::Position{2.0f, 0.0f,0.0f} };
		Vulkan::Physics::FrameHitbox gameStarter{ Vulkan::Physics::Position{2.5f, -5.95f, 0.0f}, 1.0f, Vulkan::Physics::Position{-2.0f, 0.0f,0.0f}, Vulkan::Physics::Position{2.0f, 0.0f,0.0f} };


		Vulkan::Physics::Field gravity{ Vulkan::Physics::Position{1.0f, 0.0f, -2.0f}, Vulkan::Physics::FieldFunctions::gravity<20> };
		Vulkan::Physics::Field friction{ Vulkan::Physics::Position{0.0f, 0.0f, -2.0f}, Vulkan::Physics::FieldFunctions::friction<2.0> };
		Vulkan::Physics::Field pullerForce{ Vulkan::Physics::Position{PULLER_RESTING_POSITION}, Vulkan::Physics::FieldFunctions::centralField<PULLER_PULLUP_FORCE> };


		
		//add models to universe
		Vulkan::Physics::Universe physicsUniverse{ std::vector<Vulkan::Physics::Field*>{&gravity, &friction}, +bumper1, +bumper2, +bumper3, +bumper4, +bumper5, +rightFlipper, +leftFlipper, +body, ballKiller};
		Vulkan::Physics::Universe pullerUniverse{ std::vector<Vulkan::Physics::Field*>{&pullerForce}, gameStarter, +puller };


		//add game status object
		std::vector<Vulkan::Physics::Hitbox*> balls{ &(+ball1), &(+ball2), &(+ball3) };
		std::vector<Vulkan::Physics::Hitbox*> bumpers{ &(+bumper1), &(+bumper2), &(+bumper3), &(+bumper4), &(+bumper5)};
		GameStatus gameStatus{ balls, bumpers, lights, physicsUniverse };


		//additional keyboard observer (for actions not realted to a specific object)
		Vulkan::Utilities::ConcreteKeyboardObserver additionalKeyboardObserver{ [&gameStatus](int keyPressed) {
			if (keyPressed == GLFW_KEY_M) {
				gameStatus.activateMultiball();
				}
			} };

		//add keyboard press controller
		Vulkan::Utilities::KeyboardListener keyboardController{ window, rightFlipper, leftFlipper, puller, additionalKeyboardObserver };


		//collision actions
		(+bumper1).setCollisionAction([&gameStatus, &bumper1](Vulkan::Physics::Hitbox&) {
			gameStatus.invertBumper(&+bumper1);
			});

		(+bumper2).setCollisionAction([&gameStatus, &bumper2](Vulkan::Physics::Hitbox&) {
			gameStatus.invertBumper(&+bumper2);
			});

		(+bumper3).setCollisionAction([&gameStatus, &bumper3](Vulkan::Physics::Hitbox&) {
			gameStatus.invertBumper(&+bumper3);
			});

		(+bumper4).setCollisionAction([&gameStatus, &bumper4](Vulkan::Physics::Hitbox&) {
			gameStatus.invertBumper(&+bumper4);
			});

		(+bumper5).setCollisionAction([&gameStatus, &bumper5](Vulkan::Physics::Hitbox&) {
			gameStatus.invertBumper(&+bumper5);
			});

		
		ballKiller.setCollisionAction([&gameStatus](Vulkan::Physics::Hitbox& collidingBall) {
			gameStatus.killBall(&collidingBall);
			});

		gameStarter.setCollisionAction([&gameStatus, &puller](Vulkan::Physics::Hitbox&) {
			gameStatus.startNewGame((+puller).getSpeed());
			(+puller).reset(PULLER_RESTING_POSITION);
			});



		// ================ VERTEX/INDEX BUFFERS SETUP ================

		//vertex buffers
		Vulkan::Buffers::VertexBuffer mainVertexBuffer{ virtualGpu, realGpu, (ball1.getVertices().size() + ball2.getVertices().size() + ball3.getVertices().size() + bumper1.getVertices().size() + bumper2.getVertices().size() + bumper3.getVertices().size() + bumper4.getVertices().size() + bumper5.getVertices().size() + rightFlipper.getVertices().size() + leftFlipper.getVertices().size() + body.getVertices().size() + puller.getVertices().size()) * sizeof(MyVertex)};
		Vulkan::Buffers::VertexBuffer backgroundVertexBuffer{ virtualGpu, realGpu, (skybox.getVertices().size() + point1.getVertices().size() + point10.getVertices().size() + point100.getVertices().size() + point1000.getVertices().size()) * sizeof(MyVertex) };
		mainVertexBuffer.fillBuffer(ball1, ball2, ball3, bumper1, bumper2, bumper3, bumper4, bumper5, rightFlipper, leftFlipper, body, puller);
		backgroundVertexBuffer.fillBuffer(skybox, point1, point10, point100, point1000);

		//index buffers
		Vulkan::Buffers::IndexBuffer mainIndexBuffer{ virtualGpu, realGpu, (ball1.getIndexes().size() + ball2.getIndexes().size() + ball3.getIndexes().size() + bumper1.getIndexes().size() + bumper2.getIndexes().size() + bumper3.getIndexes().size() + bumper4.getIndexes().size() + bumper5.getIndexes().size() + rightFlipper.getIndexes().size() + leftFlipper.getIndexes().size() + body.getIndexes().size() + body.getIndexes().size()) * sizeof(uint32_t)};
		Vulkan::Buffers::IndexBuffer backgroundIndexBuffer{ virtualGpu, realGpu, (skybox.getIndexes().size() + point1.getIndexes().size() + point10.getIndexes().size() + point100.getIndexes().size() + point1000.getIndexes().size()) * sizeof(uint32_t) };
		mainIndexBuffer.fillBuffer(ball1, ball2, ball3, bumper1, bumper2, bumper3, bumper4, bumper5, rightFlipper, leftFlipper, body, puller);
		backgroundIndexBuffer.fillBuffer(skybox, point1, point10, point100, point1000);






		// ================ UNIFORMS/TEXTURES SETUP ================

		//texture image
		Vulkan::TextureImage mainTexture{ virtualGpu, realGpu, commandBufferPool, std::pair(2048, 2048), "textures/Mario11.png" };
		Vulkan::TextureImage backgroundTexture{ virtualGpu, realGpu, commandBufferPool, std::pair(2048, 2048), "textures/skybox.png" };

		//uniform buffers
		Vulkan::Buffers::UniformBuffer mainGlobalUniformBuffer{ virtualGpu, realGpu, 2048 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer mainPerObjectUniformBuffer{ virtualGpu, realGpu, 1024 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer backgroundGlobalUniformBuffer{ virtualGpu, realGpu, 2048 * sizeof(float) };
		Vulkan::Buffers::UniformBuffer backgroundPerObjectUniformBuffer{ virtualGpu, realGpu, 1024 * sizeof(float) };

		//descriptor sets
		Vulkan::StaticSet mainGlobalSet{ virtualGpu, std::tuple{ VK_SHADER_STAGE_ALL, &mainTexture}, std::tuple{ VK_SHADER_STAGE_ALL, Lights{}, &mainGlobalUniformBuffer, 0 } };
		Vulkan::DynamicSet mainPerObjectSet{ realGpu, virtualGpu, mainPerObjectUniformBuffer, std::pair{VK_SHADER_STAGE_ALL, Matrices{}} };
		Vulkan::StaticSet backgroundGlobalSet{ virtualGpu, std::tuple{VK_SHADER_STAGE_ALL, &backgroundTexture} };
		Vulkan::DynamicSet backgroundPerObjectSet{ realGpu, virtualGpu, backgroundPerObjectUniformBuffer, std::pair{VK_SHADER_STAGE_ALL, Matrices{}} };






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
		Vulkan::PipelineOptions::Shader backgroundVertexShader{ virtualGpu, "shaders/BackgroundBoxShaderVert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT };
		Vulkan::PipelineOptions::Shader backgroundFragmentShader{ virtualGpu, "shaders/BackgroundBoxShaderFrag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT };

		Vulkan::Pipeline backgroundPipeline{ virtualGpu, renderPass, 0, std::vector{&backgroundVertexShader, &backgroundFragmentShader},vertexTypesDescriptor, backgroundPipelineLayout, rasterizer, inputAssembly, multisampler, depthStencil, dynamicState, viewport };





	
		//create drawer
		Vulkan::Drawer drawer{ virtualGpu, realGpu, window, windowSurface, swapchain, depthBuffer, commandBufferPool, renderPass,
			{&mainPipeline, &backgroundPipeline},
			{mainGlobalSet, backgroundGlobalSet},
			{mainPerObjectSet, backgroundPerObjectSet} };

		std::cout << "\n";
		//physics cycle in new thread
		std::thread physicsThread{ [&physicsUniverse, &pullerUniverse, &keyboardController, &leftFlipper, &rightFlipper, &window] () {
			auto lastFrameTime = std::chrono::high_resolution_clock::now();
			while (!glfwWindowShouldClose(+window)) {
				auto elapsedNano = std::chrono::high_resolution_clock::now() - lastFrameTime;
				if (elapsedNano.count() > 100) {
					lastFrameTime = std::chrono::high_resolution_clock::now();
					calculatePhysics(std::vector{ &physicsUniverse, &pullerUniverse }, keyboardController, +leftFlipper, +rightFlipper, std::chrono::nanoseconds{ elapsedNano });
				}
			}
		} };

		//draw cycle
		auto lastFrameTime = std::chrono::high_resolution_clock::now();
		while (!glfwWindowShouldClose(+window)) {
			glfwPollEvents();
			calculateGraphics(mainPerObjectUniformBuffer, mainPerObjectSet, mainGlobalUniformBuffer, mainGlobalSet, backgroundPerObjectUniformBuffer, backgroundPerObjectSet, backgroundVertexBuffer, std::tuple{ &ball1, &ball2, &ball3, &bumper1, &bumper2, &bumper3, &bumper4, &bumper5, &rightFlipper, &leftFlipper, &body, &puller, &point1, &point10, &point100, &point1000, &skybox }, lights, physicsUniverse, pullerUniverse, keyboardController, (float)swapchain.getResolution().first/swapchain.getResolution().second, gameStatus.getPoints());
			lastFrameTime = std::chrono::high_resolution_clock::now();
			drawer.draw(
				std::pair< std::reference_wrapper<Vulkan::Buffers::VertexBuffer>, std::reference_wrapper<Vulkan::Buffers::IndexBuffer>>{ mainVertexBuffer, mainIndexBuffer },
				std::pair< std::reference_wrapper<Vulkan::Buffers::VertexBuffer>, std::reference_wrapper<Vulkan::Buffers::IndexBuffer>>{ backgroundVertexBuffer, backgroundIndexBuffer }
			);
		}
		vkDeviceWaitIdle(+virtualGpu);

		physicsThread.join();

		std::cout << "\n";
	} catch (const Vulkan::VulkanException& ve) {
		std::cout << ve.what();
	}
	std::cout << "\n\n";

}



template<typename... Models>
void calculateGraphics(Vulkan::Buffers::UniformBuffer& mainPerObjectBuffer, const Vulkan::DynamicSet& mainPerObjectSet, Vulkan::Buffers::UniformBuffer& mainGlobalBuffer, const Vulkan::StaticSet& mainGlobalSet, Vulkan::Buffers::UniformBuffer& backgroundBuffer, const Vulkan::DynamicSet& backgroundSet, Vulkan::Buffers::VertexBuffer& backgroundVertexBuffer, const std::tuple<Models*...>& models, Lights& lights, Vulkan::Physics::Universe& universe, Vulkan::Physics::Universe& pullerUniverse, Vulkan::Utilities::KeyboardListener& keyboardController, float aspectRatio, int points) {
	float n = 0.1f, f = 10000.0f, fovY = 120.0f, a = aspectRatio, w = 1.0f;
	glm::mat4 perspective{
			1 / (a * glm::tan(glm::radians(fovY / 2))), 0, 0, 0,
			0, -1 / glm::tan(glm::radians(fovY / 2)), 0, 0,
			0, 0, f / (n - f), -1,
			0, 0, (n * f) / (n - f), 0
	};

	static glm::mat4 parallel {
		1 / w, 0, 0, 0,
		0, -a / w, 0, 0,
		0, 0, 1 / (n - f), 0,
		0, 0, n / (n - f), 1
	};

	auto camera = Vulkan::Objects::Camera{ {0.0f, -4.2f, 5.5f}, {0.0_deg, 60.0_deg, 0.0_deg} };
	//static auto camera = Vulkan::Objects::Camera{ {0.0f, 0.0f, 0.0f}, {0.0_deg, 0.0_deg, 0.0_deg} };
	//camera.rotate(0.001f, { 0.0f, 0.0f, 1.0f });

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
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& puller = *std::get<11>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& point1 = *std::get<12>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& point10 = *std::get<13>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& point100 = *std::get<14>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& point1000 = *std::get<15>(models);
	Vulkan::Objects::Model<Vulkan::PipelineOptions::Vertex<glm::vec3, glm::vec3, glm::vec2>>& skybox = *std::get<16>(models);


	lights.position5 = (+ball1).getPosition(); lights.position5.z = 0.16f;
	lights.position6 = (+ball2).getPosition(); lights.position6.z = 0.16f;
	lights.position7 = (+ball3).getPosition(); lights.position7.z = 0.16f;

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
		body.getUniforms(camera.getViewMatrix(), projection),
		puller.getUniforms(camera.getViewMatrix(), projection)
	);

	point1.setVertices(buildPointDisplayerVertices(points / 1 % 10));
	point10.setVertices(buildPointDisplayerVertices(points / 10 % 10));
	point100.setVertices(buildPointDisplayerVertices(points / 100 % 10));
	point1000.setVertices(buildPointDisplayerVertices(points / 1000 % 10));

	//fill background vertices (because texture coordinates of the point displayer changes)
	backgroundVertexBuffer.fillBuffer(skybox, point1, point10, point100, point1000);

	//fill uniform buffers
	backgroundSet.fillBuffer(backgroundBuffer, skybox.getUniforms(camera.getViewMatrix(), projection), point1.getUniforms(camera.getViewMatrix(), projection), point10.getUniforms(camera.getViewMatrix(), projection), point100.getUniforms(camera.getViewMatrix(), projection), point1000.getUniforms(camera.getViewMatrix(), projection));
	mainGlobalSet.fillBuffer(mainGlobalBuffer, lights);
};






void calculatePhysics(std::vector<Vulkan::Physics::Universe*> universes, Vulkan::Utilities::KeyboardListener& kc, Vulkan::Physics::Hitbox& leftFlipper, Vulkan::Physics::Hitbox& rightFlipper, std::chrono::nanoseconds elapsedNanoseconds) {
	float elapsedSeconds = elapsedNanoseconds.count() / 1000000000.0f;

	rightFlipper.setAngularSpeed(0.0f);
	if (rightFlipper.getRotationEuler()[0] < -FLIPPER_MIN_ANGLE) {
		rightFlipper.setAngularSpeed(FLIPPER_ANGULAR_SPEED);
	}
	leftFlipper.setAngularSpeed(0.0f);
	if (leftFlipper.getRotationEuler()[0] > 180.0_deg + FLIPPER_MIN_ANGLE || leftFlipper.getRotationEuler()[0] < 0.0_deg) {
		leftFlipper.setAngularSpeed(-FLIPPER_ANGULAR_SPEED);
	}
	kc.checkKeyPressed();

	for (auto universe : universes) {
		universe->calculate(elapsedSeconds);
	}
};

