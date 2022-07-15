#ifndef VULKAN_ATTACHMENT
#define VULKAN_ATTACHMENT

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanException.h"


namespace Vulkan::PipelineOptions::RenderPassOptions {
	class Attachment; enum class PredefinedAttachment; enum class AttachmentType;

	using AttachmentReferenceTuple = std::tuple<std::vector<VkAttachmentReference>, std::vector<VkAttachmentReference>, std::vector<VkAttachmentReference>, std::vector<uint32_t>>;

	template<typename T>
	concept IsAttachment = requires(T t) {
		std::same_as<T, Attachment> || std::same_as < T, std::pair<Attachment, VkImageLayout>>;
	};

}

enum class Vulkan::PipelineOptions::RenderPassOptions::PredefinedAttachment {
	STANDARD_COLOR
};

enum class Vulkan::PipelineOptions::RenderPassOptions::AttachmentType {
	INPUT, COLOR, DEPTH_STENCIL
};

class Vulkan::PipelineOptions::RenderPassOptions::Attachment {
	public:

		/**
		 * @brief Generates an attachment and his reference based on some standard attachment models.
		 * @details The attachment reference will have predefined layout and his index will always be 0.
		 * 
		 * @param predefined A set of predefined attachments.
		 * @param format The attachment format can be specified.
		 */
		Attachment(PredefinedAttachment predefined = PredefinedAttachment::STANDARD_COLOR, VkFormat format = VK_FORMAT_B8G8R8A8_SRGB) : attachment{}, attachmentReference{} {
			//struct for the attachment
			switch (predefined) {
			case PredefinedAttachment::STANDARD_COLOR:
				attachment.format = format;
				attachment.samples = VK_SAMPLE_COUNT_1_BIT;
				attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				//format reference
				attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				//set type
				type = AttachmentType::COLOR;
				break;
			}
		}


		Attachment(const VkAttachmentDescription& baseAttachment, AttachmentType type, VkImageLayout referenceLayout = VK_IMAGE_LAYOUT_UNDEFINED) : attachment{ baseAttachment }, attachmentReference{} {
			attachmentReference.layout = referenceLayout;
			this->type = type;
		}


		const VkAttachmentDescription& operator+() const {
			return attachment;
		}


		AttachmentType& getType() {
			return type;
		}


		VkImageLayout& getAttachmentReferenceLayout() {
			return attachmentReference.layout;
		}


		/**
		 * @brief Returns the array of VkAttachmentDescription and the tuple of arrays of VkAttachmentReference.
		 * @details VkAttachmentReference in the arrays of the tuple will have the indexes (VkAttachmentReference.attachment) already set, based on the position of the attachments in the VkAttachmentDescription array.
		 *			VkAttachmentReference objects will be put in the right array of the tuple based on the type of the Attachment they refer to: <input refs, color refs, depth/stencil refs, preserve indexes=empty>
		 * 
		 * @param ...attachments Attachments to generate the array from.
		 * @tparam A Each argument can be either an Attachment or a std::pair<Attachment, VkImageLayout>. The pair is used to pass a different VkAttachmentReference.layout than the one specified during attachment creation.
		 * @return The array of VkAttachmentDescription and the tuple of arrays of VkAttachmentReference.
		 */
		template<IsAttachment... A>
		static std::pair<std::vector<VkAttachmentDescription>, AttachmentReferenceTuple> prepareAttachmentsArray(A... attachments) {
			std::vector<VkAttachmentDescription> attachmentsResult; //array with all the VkAttachmentDescription of the attachments passed as argument
			AttachmentReferenceTuple attachmentReferencesTuple; //<input refs, color refs, depth/stencil refs, preserve indexes=empty>
			
			//Put in the vector the attachment and layout to bind toghether. The right overload is called based on the type of the attachment argument (pair<Attachment, layout> or Attachment).
			std::vector<std::pair<Attachment, VkImageLayout>> attachmentLayoutPairs; 		
			(attachmentLayoutPairs.push_back(addAttachment(attachments)), ...);

			//for each pair in the vector, add it to the arrays
			for (const auto& alp : attachmentLayoutPairs) {
				attachmentsResult.push_back(alp.first.attachment); //add the underlying attachment to the array to return

				VkAttachmentReference tmpAttachmentReference{};
				tmpAttachmentReference.attachment = attachmentsResult.size() - 1; //set the index of the attachment reference
				tmpAttachmentReference.layout = alp.second; //set the layout of the attachment reference

				//based on the type, add the reference to the corresponding array; this tuple is then used when creating the subpass
				switch (alp.first.type) {
				case AttachmentType::INPUT:
					std::get<0>(attachmentReferencesTuple).push_back(tmpAttachmentReference);
					break;
				case AttachmentType::COLOR:
					std::get<1>(attachmentReferencesTuple).push_back(tmpAttachmentReference);
					break;
				case AttachmentType::DEPTH_STENCIL:
					std::get<2>(attachmentReferencesTuple).push_back(tmpAttachmentReference);
					break;
				default:
					throw VulkanException("The specified AttachmentType is invalid");
					break;
				}
			}

			return std::pair{ attachmentsResult, attachmentReferencesTuple };
		}


	private:

		//These 2 functions are used to return the attachment and its respective layout, which are then used in prepareAttachmentsArray(...) to build the arrays to create a subpass.
		static std::pair<Attachment, VkImageLayout> addAttachment(const std::pair<Attachment, VkImageLayout>& attachment) {
			return attachment;
		}

		//These 2 functions are used to return the attachment and its respective layout, which are then used in prepareAttachmentsArray(...) to build the arrays to create a subpass.
		static std::pair<Attachment, VkImageLayout> addAttachment(const Attachment& attachment) {
			return std::pair{ attachment, attachment.attachmentReference.layout };
		}


		VkAttachmentDescription attachment;
		VkAttachmentReference attachmentReference;
		AttachmentType type;
};

#endif
