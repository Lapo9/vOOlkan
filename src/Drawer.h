#ifndef VULKAN_DRAWER
#define VULKAN_DRAWER

#include <vulkan/vulkan.h>
#include <vector>

#include "LogicalDevice.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "Pipeline.h"
#include "Framebuffer.h"
#include "CommandBuffer.h"
#include "CommandBufferPool.h"
#include "Window.h"
#include "Queue.h"
#include "Fence.h"
#include "Semaphore.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "DescriptorSetPool.h"
#include "DescriptorSet.h"
#include "VulkanException.h"
#include "Set.h"
#include "DynamicSet.h"
#include "StaticSet.h"


namespace Vulkan { class Drawer; }


/**
 * @brief A Drawer is a class which holds all of the resources to draw frames on screen, such as the synchronization primitives, the framebuffers, the swapchain, ... and the function to actually draw a frame.
 */
class Vulkan::Drawer {
public:

	//TODO rewrite this Doxygen
	/**
	 * @brief Creates all the resources needed to draw something on screen.
	 * @details These resources more specifically are: Swapchain, Framebuffer(s), CommandBufferPool, COmmandBuffer(s), Fence(s), Semaphore(s).
	 * 
	 * @param virtualGpu The LogicalDevice.
	 * @param realGpu The PhysiscalDevice.
	 * @param window The Window ehre to draw.
	 * @param windowSurface The WindowSurface of the window (also serves to poll for window resizes and such).
	 * @param renderPass How to draw a frame.
	 * @param pipeline The stages to draw a frame.
	 * @param framesInFlight How many frames can be rendered concurrently.
	 */
	Drawer(const LogicalDevice& virtualGpu,
		const PhysicalDevice& realGpu,
		const Window& window,
		const WindowSurface& windowSurface,
		Swapchain& swapchain,
		DepthImage& depthBuffer,
		const CommandBufferPool& commandBufferPool,
		const PipelineOptions::RenderPass& renderPass,
		std::vector<Pipeline*> pipelines,
		const std::vector<std::reference_wrapper<StaticSet>>& globalSets,
		const std::vector<std::reference_wrapper<DynamicSet>>& perObjectSets,
		unsigned int framesInFlight = 2)
		:
		framesInFlight{ framesInFlight },
		currentFrame{ 0 },
		virtualGpu{ virtualGpu },
		realGpu{ realGpu },
		window{ window },
		windowSurface{ windowSurface },
		depthBuffer{ depthBuffer },
		renderPass{ renderPass },
		pipelines{ pipelines },
		globalDescriptorSets{},
		perObjectDescriptorSets{},
		swapchain{ swapchain },
		commandBufferPool{ commandBufferPool } ,
		descriptorSetPool{ virtualGpu, framesInFlight*10, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }{

		framebuffers = Framebuffer::generateFramebufferForEachSwapchainImageView(virtualGpu, renderPass, swapchain, depthBuffer["base"]);
		//for each frame in flight instantiate the required objects
		for (unsigned int i = 0; i < framesInFlight; ++i) {
			fences.emplace_back(virtualGpu);
			imageAvailableSemaphores.emplace_back(virtualGpu);
			renderFinishedSemaphores.emplace_back(virtualGpu);
			commandBuffers.emplace_back(virtualGpu, commandBufferPool);
			globalDescriptorSets.emplace_back();
			perObjectDescriptorSets.emplace_back();

			//globalSets and perObjectSets must have the same size
			//for each pipeline instantiate the descriptor sets
			for (unsigned int j = 0; j < globalSets.size(); ++j) {
				globalDescriptorSets[i].emplace_back(virtualGpu, descriptorSetPool, globalSets[j]);
				perObjectDescriptorSets[i].emplace_back(virtualGpu, descriptorSetPool, perObjectSets[j]);
			}
		}
	}


	//TODO specify which are the global commands and which are the per vertex commands
	/**
	 * @brief Draws the vertexBuffer by running the specified commands.
	 * 
	 * @param vertexBuffer Vertices to draw.
	 * @param ...commands Vulkan Functions to execute on the vertices.
	 * @tparam ...Args The types of the arguments to pass to each Vulkan function.
	 * @tparam ...Command The tuples, each containing the Vulkan function and its arguments (of type ...Args).
	 */
	/*template<typename... Args, template<typename...> class... Command> requires (std::same_as<Command<>, std::tuple<>> && ...)
	void draw(const Buffers::VertexBuffer& vertexBuffer, const Buffers::IndexBuffer& indexBuffer, Command<void(*)(VkCommandBuffer, Args...), Args...>&&... commands) {
		uint32_t obtainedSwapchainImageIndex; //the index of the image of the swapchain we'll draw to
		vkWaitForFences(+virtualGpu, 1, &+fences[currentFrame], VK_TRUE, UINT64_MAX); //wait until a swapchain image is free
		//get an image from the swapchain
		if (VkResult result = vkAcquireNextImageKHR(+virtualGpu, +swapchain, UINT64_MAX, +imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &obtainedSwapchainImageIndex); result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
			return;
		} 
		else if (result != VK_SUCCESS) {
			throw VulkanException{ "Failed to acquire swapchain image", result };
		}
		vkResetFences(+virtualGpu, 1, &+fences[currentFrame]); //reset the just signaled fence to an unsignalled state
		
		//fill the command buffer
		commandBuffers[currentFrame].reset(renderPass, framebuffers[obtainedSwapchainImageIndex], pipeline);	
		commandBuffers[currentFrame].addCommands(std::forward<Command...>(commands...));
		commandBuffers[currentFrame].endCommand();

		//struct to submit a command buffer to a queue
		VkSemaphore waitSemaphores[] = { +imageAvailableSemaphores[currentFrame] }; //semaphore used to signal that an image is available to render to
		VkSemaphore signalSemaphores[] = { +renderFinishedSemaphores[currentFrame]}; //semaphore used to signal that the render finished
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; //where to wait for an image (first semaphore). You can still run the vertex shader without an image, but you have to wait for an image for the fragment shader
		
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &+commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (VkResult result = vkQueueSubmit(+virtualGpu[QueueFamily::GRAPHICS], 1, &submitInfo, +fences[currentFrame]); result != VK_SUCCESS) {
			throw VulkanException{ "Failed to submit the command buffer for the current frame", result };
		}


		VkSwapchainKHR swapchains[] = { +swapchain };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &obtainedSwapchainImageIndex;

		if (VkResult result = vkQueuePresentKHR(+virtualGpu[QueueFamily::PRESENTATION], &presentInfo); result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
		}
		else if (result != VK_SUCCESS) {
			throw VulkanException{ "Failed to present image", result };
		}

		increaseCurrentFrame();
	}
	*/

	//TODO probably it should be a pair of references
	/**
	 * @brief Draws the vertexBuffer.
	 * @details This function will simply bind the vertex buffer (vertexBuffer) and then draw it.
	 *
	 * @param vertexBuffer Vertices to draw.
	 * @param indexBuffer Indeces of the vertices to draw (for indexed drawing).
	 */
	template<template<typename, typename>class... P> requires (std::same_as<P<int, int>, std::pair<int, int>> && ...)
		void draw(const P<std::reference_wrapper<Buffers::VertexBuffer>, std::reference_wrapper<Buffers::IndexBuffer>>&... buffers) {
		uint32_t obtainedSwapchainImageIndex{}; //the index of the image of the swapchain we'll draw to
		vkWaitForFences(+virtualGpu, 1, &+fences[currentFrame], VK_TRUE, UINT64_MAX); //wait until a swapchain image is free
		//get an image from the swapchain
		if (VkResult result = vkAcquireNextImageKHR(+virtualGpu, +swapchain, UINT64_MAX, +imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &obtainedSwapchainImageIndex); result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
			return;
		}
		else if (result != VK_SUCCESS) {
			throw VulkanException{ "Failed to acquire swapchain image", result };
		}
		vkResetFences(+virtualGpu, 1, &+fences[currentFrame]); //reset the just signaled fence to an unsignalled state

		commandBuffers[currentFrame].reset(renderPass, framebuffers[obtainedSwapchainImageIndex]);
		//fill the command buffer (for each pipeline)
		unsigned int counter = 0;
		([&](const Buffers::VertexBuffer& vertexBuffer, const Buffers::IndexBuffer& indexBuffer) {
			VkDeviceSize offsets[] = { 0 };
			commandBuffers[currentFrame].addCommand(vkCmdBindPipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, +*pipelines[counter]);
			commandBuffers[currentFrame].addCommand(vkCmdBindVertexBuffers, 0, 1, &+vertexBuffer, offsets);
			commandBuffers[currentFrame].addCommand(vkCmdBindIndexBuffer, +indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			commandBuffers[currentFrame].addCommand(vkCmdBindDescriptorSets, VK_PIPELINE_BIND_POINT_GRAPHICS, +pipelines[counter]->getLayout(), 0, 1, &+globalDescriptorSets[currentFrame][counter], 0, nullptr); //TODO at the moment the binding of the global descriptors is dynamic, but it should be static (not a big deal really)
			for (int i = 0; i < indexBuffer.getModelsCount(); ++i) {
				commandBuffers[currentFrame].addCommand(vkCmdBindDescriptorSets, VK_PIPELINE_BIND_POINT_GRAPHICS, +pipelines[counter]->getLayout(), 1, 1, &+perObjectDescriptorSets[currentFrame][counter], perObjectDescriptorSets[currentFrame][counter].getSet().getAmountOfBindings(), perObjectDescriptorSets[currentFrame][counter].getSet().getDynamicDistances(i).data());
				commandBuffers[currentFrame].addCommand(vkCmdDrawIndexed, indexBuffer.getModelIndexesCount(i), 1, indexBuffer.getModelOffset(i), 0, 0);
			}
			counter++;
			}(buffers.first, buffers.second), ...);

		commandBuffers[currentFrame].addCommand(vkCmdEndRenderPass);
		commandBuffers[currentFrame].endCommand();

		//submit the command buffer to a queue
		std::vector<VkSemaphore> waitSemaphores = { +imageAvailableSemaphores[currentFrame] }; //semaphore used to signal that an image is available to render to
		std::vector<VkSemaphore> signalSemaphores = { +renderFinishedSemaphores[currentFrame] }; //semaphore used to signal that the render finished
		std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; //where to wait for an image (first semaphore). You can still run the vertex shader without an image, but you have to wait for an image for the fragment shader
		commandBuffers[currentFrame].sendCommand(virtualGpu[QueueFamily::GRAPHICS], waitSemaphores, signalSemaphores, waitStages, fences[currentFrame]);

		//submit the rendered image back to the swapchain for presentation
		std::vector<VkSwapchainKHR> swapchains = { +swapchain };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = signalSemaphores.size();
		presentInfo.pWaitSemaphores = signalSemaphores.data();
		presentInfo.swapchainCount = swapchains.size();
		presentInfo.pSwapchains = swapchains.data();
		presentInfo.pImageIndices = &obtainedSwapchainImageIndex;

		if (VkResult result = vkQueuePresentKHR(+virtualGpu[QueueFamily::PRESENTATION], &presentInfo); result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
		}
		else if (result != VK_SUCCESS) {
			throw VulkanException{ "Failed to present image", result };
		}

		increaseCurrentFrame();
	}

	
	

private:

	void increaseCurrentFrame() {
		currentFrame = (currentFrame + 1) % framesInFlight;
	}


	void recreateSwapchain() {
		//if window is minimized, pause application
		int width = 0, height = 0;
		do {
			glfwGetFramebufferSize(+window, &width, &height);
			glfwWaitEvents();
		} while (width == 0 || height == 0);
 
		vkDeviceWaitIdle(+virtualGpu); //wait for all the resources to be free

		//recreate new swapchain
		swapchain = Swapchain(realGpu, virtualGpu, windowSurface, window, swapchain);
		depthBuffer = DepthImage{ virtualGpu, realGpu, swapchain.getResolution() };
		framebuffers = Framebuffer::generateFramebufferForEachSwapchainImageView(virtualGpu, renderPass, swapchain, depthBuffer["base"]);
		commandBuffers = std::vector<CommandBuffer>{};

		for (unsigned int i = 0; i < framesInFlight; ++i) {
			commandBuffers.emplace_back(virtualGpu, commandBufferPool);
		}
	}

	//Each string (name) is bound to a vector of synch primitives. Each element in the vector is a different primitive, and each one is used for a specific frame in flight.
	//In reality each name is bound to a pair, where the second element is the actual primitive hold by the first one, for performance reasons.
	//std::map<std::string, std::pair<std::vector<Semaphore>, std::vector<VkSemaphore>>> semaphores;
	//std::map<std::string, std::pair<std::vector<Fence>, std::vector<VkFence>>> fences;

	unsigned int currentFrame; //indicates which set of resources to use for the current frame (0 < x < maxFramesInFlight)
	unsigned int framesInFlight; //maximum number of frames that can be rendered at the same time(of course no more than the number of swap chain images)

	const LogicalDevice& virtualGpu;
	const PhysicalDevice& realGpu;
	const Window& window;
	const WindowSurface& windowSurface;
	const PipelineOptions::RenderPass& renderPass;
	std::vector<Pipeline*> pipelines;
	const CommandBufferPool& commandBufferPool; 

	Swapchain& swapchain;
	DepthImage& depthBuffer;
	std::vector<Framebuffer> framebuffers;
	DescriptorSetPool descriptorSetPool;

	std::vector<SynchronizationPrimitives::Fence> fences;
	std::vector<SynchronizationPrimitives::Semaphore> imageAvailableSemaphores; //tells when an image is occupied by rendering
	std::vector<SynchronizationPrimitives::Semaphore> renderFinishedSemaphores; //tells when the rendeing of the image ends
	std::vector<CommandBuffer> commandBuffers;
	std::vector<std::vector<DescriptorSet<StaticSet>>> globalDescriptorSets; //descriptor sets (one per frame in flight x one per pipeline) for the global info
	std::vector<std::vector<DescriptorSet<DynamicSet>>> perObjectDescriptorSets; //descriptor sets (one per frame in flight x one per pipeline) for the per-object info
};

#endif
