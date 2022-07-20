#ifndef VULKAN_FRAMEBUFFER
#define VULKAN_FRAMEBUFFER

#include <vulkan/vulkan.h>
#include <concepts>
#include <vector>
#include <utility>
#include <deque>

#include "ImageView.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "VulkanException.h"
#include "Swapchain.h"


namespace Vulkan { class Framebuffer; }

/**
 * @brief A Framebuffer links toghether the real attachments (e.g. image views in the swapchain or depth buffers) with the attachments specified in a render pass (which are only descriptions).
 */
class Vulkan::Framebuffer {
public:
	
	template<std::same_as<ImageView>... IV>
	Framebuffer(const LogicalDevice& virtualGpu, const PipelineOptions::RenderPass& renderPass, std::pair<unsigned int, unsigned int> resolution, const IV&... imageViews) : virtualGpu{ virtualGpu }, resolution{ resolution } {
		std::vector<VkImageView> attachments;
		(attachments.push_back(+imageViews), ...);

		if (renderPass.attachmentCount != attachments.size()) {
			throw VulkanException("Failed to create framebuffer!", "The number of attachments in the frame buffer must match the number of attachment descriptions in the render pass");
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = +renderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = resolution.first;
		framebufferInfo.height = resolution.second;
		framebufferInfo.layers = 1;

		if (VkResult result = vkCreateFramebuffer(+virtualGpu, &framebufferInfo, nullptr, &framebuffer); result != VK_SUCCESS) {
			throw VulkanException("Failed to create framebuffer!");
		}

		std::cout << "\n+ Framebuffer created";
	}

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&&) = default;
	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer& operator=(Framebuffer&&) = default;

	~Framebuffer() {
		vkDestroyFramebuffer(+virtualGpu, framebuffer, nullptr);
		std::cout << "\n- Framebuffer destroyed";
	}


	const VkFramebuffer& operator+() const {
		return framebuffer;
	}


	std::pair<unsigned int, unsigned int> getResolution() const {
		return resolution;
	}


	/**
	 * @brief Creates a framebuffer for each image view in the specified swapchain. It also attaches the specified other attachments to the same framebuffer.
	 * 
	 * @param virtualGpu The LogicalDevice where to create the framebuffers.
	 * @param renderPass The RenderPass where there should be attachments to link against.
	 * @param swapchain The Swapchain where the ImageView(s) are present.
	 * @param ...otherAttachments Other attachments to bundle toghether in the framebuffer of each image view of the swapchain.
	 * @return A framebuffer vector containing framebuffers made up of each image in the swapchain + otherAttachments.
	 */
	template<std::same_as<ImageView>... IV>
	static std::deque<Framebuffer> generateFramebufferForEachSwapchainImageView(const LogicalDevice& virtualGpu, const PipelineOptions::RenderPass& renderPass, const Swapchain& swapchain, IV... otherAttachments) {
		std::deque<Framebuffer> framebuffers; //FIXTHIS we use a deque because the vector resizes itself, and by doing so it calls the destructor of Framebuffer, which, for some obscure reason fails!

		auto& images = swapchain.getImages();
		for (const auto& image : images) {
			auto& imageViews = image.getImageViews();
			for (const auto& imageView : imageViews) {
				framebuffers.emplace_back(virtualGpu, renderPass, swapchain.getResolution(), imageView.second, otherAttachments...);
			}
		}

		return framebuffers;
	}

private:
	VkFramebuffer framebuffer;
	const LogicalDevice& virtualGpu;
	std::pair<unsigned int, unsigned int> resolution;
};


#endif
