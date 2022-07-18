#ifndef VULKAN_COMMANDBUFFER
#define VULKAN_COMMANDBUFFER

#include <vulkan/vulkan.h>

#include <functional>
#include <utility>

#include "CommandBufferPool.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "VulkanException.h"


namespace Vulkan { class CommandBuffer; }

class Vulkan::CommandBuffer {
public:
	CommandBuffer(const LogicalDevice& virtualGpu, const CommandBufferPool& commandBufferPool) : virtualGpu{ virtualGpu } {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = +commandBufferPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (VkResult result = vkAllocateCommandBuffers(+virtualGpu, &allocInfo, &commandBuffer); result != VK_SUCCESS) {
			throw VulkanException("Failed to allocate command buffers!", result);
		}
	}

	
	const VkCommandBuffer& operator+() {
		return commandBuffer;
	}


	//TODO make it possible to specify Viewport and Scissor
	/**
	 * @brief Initialize a command buffer with the operations which are in common with most command buffers.
	 * 
	 * @param renderPass Used to begin the render pass.
	 * @param framebuffer Where the GPU will write.
	 * @param pipeline What pipeline to use.
	 */
	void reset(const PipelineOptions::RenderPass& renderPass, const Framebuffer& framebuffer, const Pipeline& pipeline) {
		//initialize command buffer
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;
		if (VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo); result != VK_SUCCESS) {
			throw VulkanException("Failed to begin recording command buffer!", result);
		}

		//set render pass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = +renderPass;
		renderPassInfo.framebuffer = +framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = VkExtent2D{ framebuffer.getResolution().first, framebuffer.getResolution().second };
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//set pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, +pipeline);

		//set viewport
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(framebuffer.getResolution().first);
		viewport.height = static_cast<float>(framebuffer.getResolution().second);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		//set scissors
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = VkExtent2D{ framebuffer.getResolution().first, framebuffer.getResolution().second };
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}


	/**
	 * @brief Adds a command to the command buffer.
	 * 
	 * @param command The operation to add.
	 * @param ...args The argumets to perform the specified operation.
	 * @tparam ...Args The types of the arguments to perform the specified operation.
	 */
	template<typename... Args>
	void addCommand(std::function<void(VkCommandBuffer, Args...)> command, Args&&... args) {
		command(commandBuffer, std::forward(args)...);
	}


	void endCommand() {
		vkCmdEndRenderPass(commandBuffer);

		if (VkResult result = vkEndCommandBuffer(commandBuffer); result != VK_SUCCESS) {
			throw VulkanException("Failed to record command buffer!", result);
		}
	}

private:
	VkCommandBuffer commandBuffer;
	const LogicalDevice& virtualGpu;
};

#endif
