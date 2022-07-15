#ifndef VULKAN_ATTACHMENT
#define VULKAN_ATTACHMENT

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanException.h"


namespace Vulkan::PipelineOptions::RenderPassOptions {
	class Attachment; enum class PredefinedAttachment; enum class AttachmentType;

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
		Attachment(PredefinedAttachment predefined = PredefinedAttachment::STANDARD_COLOR, VkFormat format = VK_FORMAT_B8G8R8A8_SRGB) : attachment{}, attachmentReferenceLayout{} {
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
				attachmentReferenceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				//set type
				type = AttachmentType::COLOR;
				break;
			}
		}


		Attachment(const VkAttachmentDescription& baseAttachment, AttachmentType type, VkImageLayout referenceLayout = VK_IMAGE_LAYOUT_UNDEFINED) : attachment{ baseAttachment }, attachmentReferenceLayout{ referenceLayout }, type{ type } {}


		const VkAttachmentDescription& operator+() const {
			return attachment;
		}


		AttachmentType& getType() {
			return type;
		}


		VkImageLayout& getAttachmentReferenceLayout() {
			return attachmentReferenceLayout;
		}


		class BoundAttachment {
		public:
			BoundAttachment(const Attachment& attachment, int index, VkImageLayout layout) : attachment{ attachment.attachment }, attachmentReference{}, type{ attachment.type } {
				attachmentReference.attachment = index;
				attachmentReference.layout = layout;
			}


			int getAttachmentReferenceIndex() const {
				return attachmentReference.attachment;
			}


			const AttachmentType& getType() const {
				return type;
			}


			const VkAttachmentReference& getAttachmentReference() const {
				return attachmentReference;
			}

		private:
			VkAttachmentDescription attachment;
			VkAttachmentReference attachmentReference;
			AttachmentType type;
		};


		/**
		 * @brief Returns the array of Attachment and the array of BoundAttachment.
		 * @details vector<BoundAttachment> is the array containing attachments with a complete VkAttachmentReference. This means that the index (VkAttachmentReference.attachment) is set based on the position of the attachments in the VkAttachmentDescription array.
		 * 
		 * @param ...attachments Attachments to generate the array from.
		 * @tparam A Each argument can be either an Attachment or a std::pair<Attachment, VkImageLayout>. The pair is used to pass a different VkAttachmentReference.layout than the one specified during attachment creation.
		 * @return The array of VkAttachmentDescription and the array of BoundAttachment.
		 */
		template<IsAttachment... A>
		static std::pair<std::vector<Attachment>, std::vector<BoundAttachment>> prepareAttachmentsArray(A... attachments) {
			std::vector<Attachment> attachmentsResult; //array with all the VkAttachmentDescription of the attachments passed as argument
			std::vector<BoundAttachment> boundAttachments; //array with the attachments which must have a complete VkAttachmentReference
			
			//Put in the vector the attachment and layout to bind toghether. The right overload is called based on the type of the attachment argument (pair<Attachment, layout> or Attachment).
			std::vector<std::pair<Attachment, VkImageLayout>> attachmentLayoutPairs; 		
			(attachmentLayoutPairs.push_back(parseAttachment(attachments)), ...);

			//for each pair in the vector, add it to the arrays
			for (const auto& alp : attachmentLayoutPairs) {
				attachmentsResult.push_back(alp.first); //add the attachment to the array to return
				boundAttachments.emplace_back(alp.first, attachmentsResult.size() - 1, alp.second); //add the BoundAttachment to the array to return
			}

			return std::pair{ attachmentsResult, boundAttachments };
		}


	private:

		//These 2 functions are used to return the attachment and its respective layout, which are then used in prepareAttachmentsArray(...) to build the arrays to create a subpass.
		static std::pair<Attachment, VkImageLayout> parseAttachment(const std::pair<Attachment, VkImageLayout>& attachment) {
			return attachment;
		}

		//These 2 functions are used to return the attachment and its respective layout, which are then used in prepareAttachmentsArray(...) to build the arrays to create a subpass.
		static std::pair<Attachment, VkImageLayout> parseAttachment(const Attachment& attachment) {
			return std::pair{ attachment, attachment.attachmentReferenceLayout };
		}


		VkAttachmentDescription attachment;
		VkImageLayout attachmentReferenceLayout;
		AttachmentType type;
};

#endif
