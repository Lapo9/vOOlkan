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


/**
 * @brief Each Attachment, based on his properties, can be used by a render Subpass in different ways: these are those ways.
 */
enum class Vulkan::PipelineOptions::RenderPassOptions::AttachmentType {
	INPUT, COLOR, DEPTH_STENCIL
};


/**
 * @brief An Attachment is basically the render target where the GPU will draw.
 * @details Lists of attachments are passed to the RenderPass and to the render Subpass(es). 
 *			The attachments passed to the render pass must be in an arbitrary but fixed order. This is because each render subpass must reference a subset of the attachments of its render pass, and to do so they use indexes, therefore the order in the render pass must be fixed.
 */
class Vulkan::PipelineOptions::RenderPassOptions::Attachment {
	public:

		/**
		 * @brief Generates an attachment based on some standard attachment models.
		 * @details The attachment will also have a predefined VkImageLayout, which will be the standard layout used to create a BoundAttachment object (but can be customized at creation).
		 * 
		 * @param predefined A set of predefined attachments.
		 */
		Attachment(PredefinedAttachment predefined = PredefinedAttachment::STANDARD_COLOR) : attachment{}, attachmentReferenceLayout{} {
			//struct for the attachment
			switch (predefined) {
			case PredefinedAttachment::STANDARD_COLOR:
				attachment.format = VK_FORMAT_B8G8R8A8_SRGB;
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


		/**
		 * @brief Creates the Attachment starting from its Vulkan underlying struct.
		 * 
		 * @param baseAttachment The Vulkan underlying struct.
		 * @param type The type of this assignment, namely how the attachment will be used by a render Subpass.
		 * @param referenceLayout Default VkImageLayout used during the creation of a BoundAttachment object if left unspecified.
		 */
		Attachment(const VkAttachmentDescription& baseAttachment, AttachmentType type, VkImageLayout referenceLayout = VK_IMAGE_LAYOUT_UNDEFINED) : attachment{ baseAttachment }, attachmentReferenceLayout{ referenceLayout }, type{ type } {}


		/**
		 * @brief Access to the underlying VkAttachmentDescription Vulkan object.
		 */
		VkAttachmentDescription& operator+() {
			return attachment;
		}


		/**
		 * @brief Returns a modifiable reference to the type of this attachment, namely how the attachment will be used by a render Subpass.
		 * 
		 * @return A modifiable reference to the type of this attachment, namely how the attachment will be used by a render Subpass.
		 */
		AttachmentType& getType() {
			return type;
		}


		/**
		 * @brief Returns a modifiable reference to the default VkImageLayout used during BoundAttachment creation if left unspecified.
		 * 
		 * @return A modifiable reference to the default VkImageLayout used during BoundAttachment creation if left unspecified.
		 */
		VkImageLayout& getAttachmentReferenceLayout() {
			return attachmentReferenceLayout;
		}


		/**
		 * @brief A BoundAttachment is an Attachment which ir ready to be used in a RenderPass and render Subpass. This basically means that a BoundAttachment is an attachment with a valid index (named VkAttachmentReference.attachment).
		 * @details The index is stored in the VkAttachmentReference object, which is a struct used by a render subpass to reference a specific attachment of its render pass.
		 *			In addition to the index the VkAttachmentReference object stores a VkImageLayout.
		 */
		class BoundAttachment {
		public:
			/**
			 * @brief Creates an object starting from an Attachment.
			 * 
			 * @param attachment Base Attachment.
			 * @param index The index with which this attachment will be referenced by a render Subpass.
			 * @param layout How the subpass will treat this attachment.
			 */
			BoundAttachment(const Attachment& attachment, int index, VkImageLayout layout) : attachment{ attachment.attachment }, attachmentReference{}, type{ attachment.type } {
				attachmentReference.attachment = index;
				attachmentReference.layout = layout;
			}


			/**
			* @brief Access to the underlying VkAttachmentDescription Vulkan object.
			*/
			const VkAttachmentDescription& operator+() const {
				return attachment;
			}


			/**
			 * @brief Returns the index with which this attachment will be referenced by a render Subpass.
			 * 
			 * @return The index with which this attachment will be referenced by a render Subpass.
			 */
			int getAttachmentReferenceIndex() const {
				return attachmentReference.attachment;
			}


			/**
			 * @brief Returns the type of this attachment, namely how the attachment will be used by a render Subpass.
			 *
			 * @return The type of this attachment, namely how the attachment will be used by a render Subpass.
			 */
			const AttachmentType& getType() const {
				return type;
			}


			/**
			 * @brief Returns the underlying VkAttachmentReference struct, which is the struct used by a render Subpass to gather information about its RenderPass attachments.
			 *
			 * @return  The underlying VkAttachmentReference struct..
			 */
			const VkAttachmentReference& getAttachmentReference() const {
				return attachmentReference;
			}

		private:
			VkAttachmentDescription attachment;
			VkAttachmentReference attachmentReference;
			AttachmentType type;
		};


		/**
		 * @brief Prepares the attachments to be used into a RenderPass and its render Subpass(es), fixing a position for each attachment and assigning this position to the relative BoundAttachment.
		 * @details vector<BoundAttachment> is the array containing attachments with a complete VkAttachmentReference. This means that the index (VkAttachmentReference.attachment) is set based on the position of the attachments in the argument list.
		 * 
		 * @param ...attachments Attachment(s) to generate the array of BoundAttachment(s) from. The order defines the fixed indexes of each BoundAttachment.
		 * @tparam A Each argument can be either an Attachment or a std::pair<Attachment, VkImageLayout>. The pair is used to pass a different VkAttachmentReference.layout to the BoundAttachment ctor than the one specified during attachment creation (the default one).
		 * @return The array of BoundAttachment(s).
		 */
		template<IsAttachment... A>
		static std::vector<BoundAttachment> prepareAttachments(A... attachments) {
			std::vector<BoundAttachment> boundAttachments; //array with the attachments which must have a complete VkAttachmentReference
			
			//Put in the vector the attachment and layout to bind toghether. The right overload is called based on the type of the attachment argument (pair<Attachment, layout> or Attachment).
			std::vector<std::pair<Attachment, VkImageLayout>> attachmentLayoutPairs; 		
			(attachmentLayoutPairs.push_back(parseAttachment(attachments)), ...);

			//for each pair in the vector, add it to the array to return
			for (const auto& alp : attachmentLayoutPairs) {
				boundAttachments.emplace_back(alp.first, boundAttachments.size(), alp.second); //add the BoundAttachment to the array to return
			}

			return boundAttachments;
		}


	private:

		//These 2 functions are used to return the attachment and its respective layout, which are then used in prepareAttachments(...) to build the array to create a subpass.
		static std::pair<Attachment, VkImageLayout> parseAttachment(const std::pair<Attachment, VkImageLayout>& attachment) {
			return attachment;
		}

		//These 2 functions are used to return the attachment and its respective layout, which are then used in prepareAttachments(...) to build the array to create a subpass.
		static std::pair<Attachment, VkImageLayout> parseAttachment(const Attachment& attachment) {
			return std::pair{ attachment, attachment.attachmentReferenceLayout };
		}


		VkAttachmentDescription attachment;
		VkImageLayout attachmentReferenceLayout;
		AttachmentType type;
};

#endif
