#ifndef VULKAN_COMMANDBUFFER
#define VULKAN_COMMANDBUFFER

#include <vulkan/vulkan.h>

#include <functional>
#include <utility>
#include <concepts>

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
		allocateCommandBuffer(virtualGpu, commandBufferPool);
	}


	/**
	 * @brief Create a command buffer and register the specified commands.
	 * @details Other than the commands registered, the framebuffer will also have the commands added by the reset and endCommand functions.
	 * 
	 * @param virtualGpu The LogicalDevice where to execute this command buffer when needed.
	 * @param commandBufferPool The pool from which obtain the empty command buffer.
	 * @param renderPass The RenderPass that will be used by this command.
	 * @param framebuffer The Framebuffer that will be used by this command.
	 * @param pipeline The Pipeline that will be used by thi command.
	 * @param ...commands Commands to register in this command buffer. Each command is made up of the Vulkan function + its arguments. The functions will be called in order and the arguments will be perfectly forwarded.
	 * @tparam ...Args The types of the arguments to pass to each Vulkan function.
	 * @tparam ...Command The tuples, each containing the Vulkan function and its arguments (of type ...Args).
	 */
	template<typename... Args, template<typename...> class... Command> requires (std::same_as<Command<int>, std::tuple<int>> && ...)
	CommandBuffer(const LogicalDevice& virtualGpu, const CommandBufferPool& commandBufferPool, const PipelineOptions::RenderPass& renderPass, const Framebuffer& framebuffer, const Pipeline& pipeline, Command<void(*)(VkCommandBuffer, Args...), Args...>&&... commands) : virtualGpu{virtualGpu} {
		allocateCommandBuffer(virtualGpu, commandBufferPool);
		reset(renderPass, framebuffer, pipeline);

		//this is tricky... call the function addCommand and pass as arguments (perfect forwarding) the objects in the each tuple received as argument
		(std::apply([this]<typename... Args>(Args&&... args) {
			this->addCommand(std::forward<Args>(args)...);
			}, commands), ...);

		endCommand();
	}

	//FIXTHIS probably implement custom move ctor

	
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
		//reset command buffer
		vkResetCommandBuffer(commandBuffer, 0);
		
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
	 * @details We differentiate ...Args and ...Params because of the perfect forwarding. 
	 *			Basically ...Params are the parameters that the function (the first parameter) wants to receive, whereas ...Args are the actual arguments that will be forwarded to this function.
	 *			It can be that the function wants an int, but the argument is deduced as an int&: without this split the overload resolution would fail:
	 *			Imagine:
	 *			@code template<typename ...T> void foo(void(*command)(T...), T&&... args)
	 *			And:
	 *			@code void bar(int x)
	 *			Then:
	 *			@code int a = 9; foo(bar, a);
	 *			would fail, because T = int& (deduced from a), but bar is void(*)(int). Therefore T = int& and T = int, which is impossible.
	 * 
	 * @param command The operation to add.
	 * @param ...args The argumets to perform the specified operation.
	 * @tparam ...Args The types of the arguments to perform the specified operation.
	 * @tparam ...Params The types of the arguments to perform the specified operation.
	 */
	template<typename... Args, typename... Params>
	void addCommand(void(*command)(VkCommandBuffer, Params...), Args&&... args) {
		command(commandBuffer, std::forward<Args>(args)...); 
	}


	/**
	 * @brief Adds the specified commands to the command buffer.
	 *
	 * @param ...commands Commands to register in this command buffer. Each command is made up of the Vulkan function + its arguments. The functions will be called in order and the arguments will be perfectly forwarded.
	 * @tparam ...Args The types of the arguments to pass to each Vulkan function.
	 * @tparam ...Command The tuples, each containing the Vulkan function and its arguments (of type ...Args).
	 */
	template<typename... Args, template<typename...> class... Command> requires (std::same_as<Command<int>, std::tuple<int>> && ...)
		void addCommands(Command<void(*)(VkCommandBuffer, Args...), Args...>&&... commands) {
		//this is tricky... call the function addCommand and pass as arguments (perfect forwarding) the objects in the each tuple received as argument
		(std::apply([this]<typename... Args>(Args&&... args) {
			this->addCommand(std::forward<Args>(args)...);
		}, commands), ...);
	}


	void endCommand() {
		vkCmdEndRenderPass(commandBuffer);

		if (VkResult result = vkEndCommandBuffer(commandBuffer); result != VK_SUCCESS) {
			throw VulkanException("Failed to record command buffer!", result);
		}
	}

private:

	void allocateCommandBuffer(const LogicalDevice& virtualGpu, const CommandBufferPool& commandBufferPool) {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = +commandBufferPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (VkResult result = vkAllocateCommandBuffers(+virtualGpu, &allocInfo, &commandBuffer); result != VK_SUCCESS) {
			throw VulkanException("Failed to allocate command buffers!", result);
		}
	}


	VkCommandBuffer commandBuffer;
	const LogicalDevice& virtualGpu;
};


#endif
