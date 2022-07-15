#ifndef VULKAN_SUBPASS
#define VULKAN_SUBPASS

#include <vulkan/vulkan.h>
#include <vector>
#include <tuple>

#include "Attachment.h"
#include "VulkanException.h"


namespace Vulkan::PipelineOptions::RenderPassOptions {
	class Subpass;

	template<typename T>
	concept IsBoundAttachment = requires(T t) {
		std::same_as<T, Attachment::BoundAttachment> || std::same_as<T, std::pair<Attachment::BoundAttachment, bool>>;
	};

	using AttachmentReferenceTuple = std::tuple<std::vector<VkAttachmentReference>, std::vector<VkAttachmentReference>, std::vector<VkAttachmentReference>, std::vector<uint32_t>>;
}

class Vulkan::PipelineOptions::RenderPassOptions::Subpass {
	public:

		template<IsBoundAttachment... BA>
		Subpass(VkPipelineBindPoint bindPoint , const BA&... boundAttachments) : subpass{} {
			//put each assignment in the correct array
			auto [inputs, colors, depthStencils, preserves] = buildAttachmentsTypesArrays(boundAttachments...);
			
			//create the structure
			subpass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS; //bindPoint;
			subpass.inputAttachmentCount = inputs.size();
			subpass.pInputAttachments = inputs.empty() ? NULL : inputs.data();
			subpass.colorAttachmentCount = colors.size();
			subpass.pColorAttachments = colors.empty() ? NULL : colors.data();;
			subpass.pDepthStencilAttachment = depthStencils.empty() ? NULL : depthStencils.data();;
			subpass.preserveAttachmentCount = preserves.size();
			subpass.pPreserveAttachments = preserves.empty() ? NULL : preserves.data();
		}


		Subpass(VkSubpassDescription baseSubpass) : subpass{ baseSubpass } {}


		const VkSubpassDescription& operator+() {
			return subpass;
		}

	private:

		//given the BoundAttachment(s), it extract their references and put them into 3 arrays, based on the type of the Attachment: input ref, color ref, depth/stencil ref
		template<IsBoundAttachment... BA>
		static AttachmentReferenceTuple buildAttachmentsTypesArrays(const BA&... boundAttachments) {
			AttachmentReferenceTuple result;

			//create a vector containing the BoundAttachment and a bool (true if that bound attachment should be added to the preserve array)
			std::vector<std::pair<Attachment::BoundAttachment, bool>> parsedBoundAttachments;
			(parsedBoundAttachments.push_back(parseBoundAttachment(boundAttachments)), ...);

			//for each BoundAttachment
			for (const auto& boundAttachment : parsedBoundAttachments) {
				//add it to the correct array: <input, color, depth/stencil>
				switch (boundAttachment.first.getType()) {
				case AttachmentType::INPUT:
					std::get<0>(result).push_back(boundAttachment.first.getAttachmentReference());
					break;
				case AttachmentType::COLOR:
					std::get<1>(result).push_back(boundAttachment.first.getAttachmentReference());
					break;
				case AttachmentType::DEPTH_STENCIL:
					std::get<2>(result).push_back(boundAttachment.first.getAttachmentReference());
					break;
				default:
					throw VulkanException("BoundAttachment type not valid");
					break;
				}

				//if the BoundAttachment should be preserved, add its index (VkAttachmentReference.attachment) to the preserve array
				if (boundAttachment.second) {
					std::get<3>(result).push_back(boundAttachment.first.getAttachmentReferenceIndex());
				}
			}

			return result;
		}


		static std::pair<Attachment::BoundAttachment, bool> parseBoundAttachment(const Attachment::BoundAttachment& boundAttachment) {
			return { boundAttachment, false };
		}

		static std::pair<Attachment::BoundAttachment, bool> parseBoundAttachment(const std::pair<Attachment::BoundAttachment, bool>& boundAttachment) {
			return { boundAttachment.first, boundAttachment.second };
		}


		VkSubpassDescription subpass;
};

#endif

