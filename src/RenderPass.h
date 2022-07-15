#ifndef VULKAN_RENDERPASS
#define VULKAN_RENDERPASS

#include <vulkan/vulkan.h>
#include <vector>
#include <concepts>

#include "Attachment.h"
#include "Subpass.h"
#include "LogicalDevice.h"
#include "VulkanException.h"


namespace Vulkan::PipelineOptions { class RenderPass; }

class Vulkan::PipelineOptions::RenderPass {
	public:
		template<std::same_as<RenderPassOptions::Subpass>... S>
		RenderPass(const LogicalDevice& virtualGpu, const std::vector<RenderPassOptions::Attachment>& attachments, const S&... subpasses) {
			//create the VkAttachmentDescription array
			std::vector<VkAttachmentDescription> attachmentDescriptions;
			for (const auto& attachment : attachments) {
				attachmentDescriptions.push_back(+attachment);
			}

			//create the VkSubpassDescription array
			std::vector<VkSubpassDescription> subpassesDescriptions;
			([&subpassesDescriptions](RenderPassOptions::Subpass subpass) {
				subpassesDescriptions.push_back(+subpass);
				}(subpasses), ...);

			//struct with the options for this render pass
			VkRenderPassCreateInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = attachmentDescriptions.size();
			renderPassInfo.pAttachments = attachmentDescriptions.data();
			renderPassInfo.subpassCount = subpassesDescriptions.size();
			renderPassInfo.pSubpasses = subpassesDescriptions.data();

			if (VkResult result = vkCreateRenderPass(+virtualGpu, &renderPassInfo, nullptr, &renderPass); result != VK_SUCCESS) {
				throw VulkanException("Failed to create render pass!", result);
			}
		}

	private:
		VkRenderPass renderPass;
};


#endif

