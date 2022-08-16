#ifndef VULKAN_SUBPASS
#define VULKAN_SUBPASS

#include <vulkan/vulkan.h>
#include <vector>
#include <tuple>

#include "Attachment.h"
#include "AttachmentColorBlendingMode.h"
#include "VulkanException.h"


namespace Vulkan::PipelineOptions::RenderPassOptions {
	class Subpass;

	template<typename T>
	concept IsBoundAttachment = requires(T t) {
		std::same_as<T, AttachmentDescription::BoundAttachmentDescription> || std::same_as<T, std::pair<AttachmentDescription::BoundAttachmentDescription, bool>>;
	};
}


/**
 * @brief A Subpass is a step of a RenderPass.
 * @details Each Subpass has a subset of the attachments (the render targets) of its father RenderPass.
 *			These attachments are divided into 3 categories based on their properties (and therefore their future use): input, color, depth/stencil. Moreover there can be specified a set of attachments to leave untouched during this subpass.
 */
class Vulkan::PipelineOptions::RenderPassOptions::Subpass {
	public:

		/**
		 * @brief Creates a Subpass with the specified attachments.
		 * @details The attachments must be present in the father RenderPass, and their index must be correct. This is ensured if the BoundAttachmentDescription(s) are part of: res = AttachmentDescription::prepareAttachments, and res is used to build the father RenderPass.
		 * 
		 * @param bindPoint How this subpass will be used.
		 * @param ...boundAttachments The attachments that this render Subpass will use.
		 * @tparam BA The ...boundAttachments can be either standard BoundAttachmentDescription(s) or std::pair<BoundAttachmentDescription, bool>. The pair is used to specify whether the attachment should be left untouched during this render subpass (by default it is false).
		 */
		template<IsBoundAttachment... BA>
		Subpass(VkPipelineBindPoint bindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, const BA&... boundAttachments) : subpass{} {
			//put each attachment in the correct array
			buildAttachmentsTypesArrays(boundAttachments...);
			
			//create the structure
			subpass.pipelineBindPoint = bindPoint;
			subpass.inputAttachmentCount = inputRefs.size();
			subpass.pInputAttachments = inputRefs.empty() ? nullptr : inputRefs.data();
			subpass.colorAttachmentCount = colorRefs.size();
			subpass.pColorAttachments = colorRefs.empty() ? nullptr : colorRefs.data();;
			subpass.pDepthStencilAttachment = depthStencilRefs.empty() ? nullptr : depthStencilRefs.data();;
			subpass.preserveAttachmentCount = preserveRefs.size();
			subpass.pPreserveAttachments = preserveRefs.empty() ? nullptr : preserveRefs.data();
		}


		/**
		 * @brief Creates a Subpass starting from the underlying Vulkan object.
		 * 
		 * @param baseSubpass The underlying Vulkan object.
		 */
		Subpass(VkSubpassDescription baseSubpass) : subpass{ baseSubpass } {}


		/**
		 * @brief Returns the underlying Vulkan VkSubpassDescription.
		 * 
		 * @return  The underlying Vulkan VkSubpassDescription.
		 */
		const VkSubpassDescription& operator+() {
			return subpass;
		}


		/**
		 * @brief Returns the VkPipelineColorBlendAttachmentState descriptors of the color attachments of this subpass.
		 * 
		 * @return The VkPipelineColorBlendAttachmentState descriptors of the color attachments of this subpass..
		 */
		std::vector<VkPipelineColorBlendAttachmentState> getColorBlendingDescriptors() const {
			std::vector<VkPipelineColorBlendAttachmentState> result;
			for (const auto& acbm : colorBlendingModes) {
				result.push_back(+acbm);
			}
			return result;
		}

	private:

		//given the BoundAttachmentDescription(s), it extract their references and put them into 3 arrays, based on the type of the AttachmentDescription: input ref, color ref, depth/stencil ref
		template<IsBoundAttachment... BA>
		void buildAttachmentsTypesArrays(const BA&... boundAttachments) {
			//create a vector containing the BoundAttachmentDescription and a bool (true if that bound attachment should be added to the preserve array)
			std::vector<std::pair<AttachmentDescription::BoundAttachmentDescription, bool>> parsedBoundAttachments;
			(parsedBoundAttachments.push_back(parseBoundAttachment(boundAttachments)), ...);

			//for each BoundAttachmentDescription
			for (const auto& boundAttachment : parsedBoundAttachments) {
				//if the BoundAttachmentDescription should be preserved, add its index (VkAttachmentReference.attachment) to the preserve array
				if (boundAttachment.second) {
					preserveRefs.push_back(boundAttachment.first.getAttachmentReferenceIndex());
				}
				else {
					//add it to the correct array: <input, color, depth/stencil>
					switch (boundAttachment.first.getType()) {
					case AttachmentType::INPUT:
						inputRefs.push_back(boundAttachment.first.getAttachmentReference());
						break;
					case AttachmentType::COLOR:
						colorRefs.push_back(boundAttachment.first.getAttachmentReference());
						colorBlendingModes.push_back(boundAttachment.first.getColorBlendingMode());
						break;
					case AttachmentType::DEPTH_STENCIL:
						depthStencilRefs.push_back(boundAttachment.first.getAttachmentReference());
						break;
					default:
						throw VulkanException("BoundAttachmentDescription type not valid");
						break;
					}
				}
			}
		}


		//These 2 functions are used to create standard pairs of bound attachments and whether they should be left untouched during this render subpass
		static std::pair<AttachmentDescription::BoundAttachmentDescription, bool> parseBoundAttachment(const AttachmentDescription::BoundAttachmentDescription& boundAttachment) {
			return { boundAttachment, false }; //by deafult the attachment is not part of the preserve set
		}

		//These 2 functions are used to create standard pairs of bound attachments and whether they should be left untouched during this render subpass
		static std::pair<AttachmentDescription::BoundAttachmentDescription, bool> parseBoundAttachment(const std::pair<AttachmentDescription::BoundAttachmentDescription, bool>& boundAttachment) {
			return { boundAttachment.first, boundAttachment.second };
		}


		VkSubpassDescription subpass;

		//arrays containing the references to each AttachmentType of attachment
		std::vector<VkAttachmentReference> inputRefs;
		std::vector<VkAttachmentReference> colorRefs;
		std::vector<VkAttachmentReference> depthStencilRefs;

		std::vector<uint32_t> preserveRefs; //contains the indexes of the attachments to leave untouched during this render pass

		std::vector<AttachmentColorBlendingMode> colorBlendingModes; //the blend mode for each color attachment
};

#endif

