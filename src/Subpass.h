#ifndef VULKAN_SUBPASS
#define VULKAN_SUBPASS

#include <vulkan/vulkan.h>
#include <vector>
#include <tuple>


namespace Vulkan::PipelineOptions::RenderPassOptions { class Subpass; }

class Vulkan::PipelineOptions::RenderPassOptions::Subpass {
	public:

		Subpass(VkPipelineBindPoint bindPoint, const std::tuple<std::vector<VkAttachmentReference>, std::vector<VkAttachmentReference>, std::vector<VkAttachmentReference>, std::vector<uint32_t>>& attachmentReferences) : subpass{} {
			subpass.pipelineBindPoint = bindPoint;
			subpass.inputAttachmentCount = std::get<0>(attachmentReferences).size();
			subpass.pInputAttachments = std::get<0>(attachmentReferences).empty() ? NULL : std::get<0>(attachmentReferences).data();
			subpass.colorAttachmentCount = std::get<1>(attachmentReferences).size();
			subpass.pColorAttachments = std::get<1>(attachmentReferences).empty() ? NULL : std::get<1>(attachmentReferences).data();;
			subpass.pDepthStencilAttachment = std::get<2>(attachmentReferences).empty() ? NULL : std::get<2>(attachmentReferences).data();;
			subpass.preserveAttachmentCount = std::get<2>(attachmentReferences).size();
			subpass.pPreserveAttachments = std::get<3>(attachmentReferences).empty() ? NULL : std::get<3>(attachmentReferences).data();
		}


		const VkSubpassDescription& operator+() {
			return subpass;
		}

	private:
		VkSubpassDescription subpass;
};

#endif

