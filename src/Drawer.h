#ifndef VULKAN_DRAWER
#define VULKAN_DRAWER

#include <vulkan/vulkan.h>
#include <vector>

#include "LogicalDevice.h"
#include "Swapchain.h"
#include "CommandBuffer.h"
#include "VulkanException.h"
#include "Queue.h"
#include "Fence.h"
#include "Semaphore.h"


namespace Vulkan { class Drawer; }


/**
 * @brief A Drawer is a class which holds all of the resources to draw frames on screen, such as the synchronization primitives, the framebuffers and the function to actually draw a frame.
 */
class Vulkan::Drawer {
public:
	Drawer(const LogicalDevice& virtualGpu, const Swapchain& swapchain, unsigned int framesInFlight = 2) :
		virtualGpu{ virtualGpu }, swapchain{ swapchain }, currentFrame{ 0 } {
		for (unsigned int i = 0; i < framesInFlight; ++i) {
			fences.emplace_back(virtualGpu);
			imageAvailableSemaphores.emplace_back(virtualGpu);
			renderFinishedSemaphores.emplace_back(virtualGpu);
		}
	}


	void draw() {
		uint32_t obtainedSwapchainImageIndex; //the index of the image of the swapchain we'll draw to
		vkWaitForFences(+virtualGpu, 1, &+fences[currentFrame], VK_TRUE, UINT64_MAX); //wait until a swapchain image is free
		vkResetFences(+virtualGpu, 1, &+fences[currentFrame]); //reset the just signalled fence to an unsignalled state
		vkAcquireNextImageKHR(+virtualGpu, +swapchain, UINT64_MAX, +imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &obtainedSwapchainImageIndex); //get an image from the swapchain
		
		//TODO the next 2 instructions should probably go in a draw() overload which takes a set of commands to register
		//vkResetCommandBuffer(+commandBuffers[currentFrame], 0);
		//recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

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

		increaseCurrentFrame();
	}


	
	

private:

	void increaseCurrentFrame() {
		currentFrame = (currentFrame + 1) % framesInFlight;
	}

	//Each string (name) is bound to a vector of synch primitives. Each element in the vector is a different primitive, and each one is used for a specific frame in flight.
	//In reality each name is bound to a pair, where the second element is the actual primitive hold by the first one, for performance reasons.
	//std::map<std::string, std::pair<std::vector<Semaphore>, std::vector<VkSemaphore>>> semaphores;
	//std::map<std::string, std::pair<std::vector<Fence>, std::vector<VkFence>>> fences;

	std::vector<SynchronizationPrimitives::Fence> fences;
	std::vector<SynchronizationPrimitives::Semaphore> imageAvailableSemaphores; //tells when an image is occupied by rendering
	std::vector<SynchronizationPrimitives::Semaphore> renderFinishedSemaphores; //tells when the rendeing of the image ends
	std::vector<CommandBuffer> commandBuffers;

	unsigned int currentFrame; //indicates which set of resources to use for the current frame (0 < x < maxFramesInFlight)
	unsigned int framesInFlight; //maximum number of frames that can be rendered at the same time(of course no more than the number of swap chain images)

	const LogicalDevice& virtualGpu;
	const Swapchain& swapchain;
};

#endif
