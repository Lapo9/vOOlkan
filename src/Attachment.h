#ifndef VULKAN_ATTACHMENT
#define VULKAN_ATTACHMENT

#include <vulkan/vulkan.h>
#include <vector>


namespace Vulkan::PipelineOptions::RenderPassOptions {
	class Attachment; enum class AttachmentType;

	template<typename T>
	concept IsAttachment = requires(T t) {
		std::same_as<T, Attachment> || std::same_as < T, std::pair<Attachment, VkImageLayout>>;
	};

}

enum class Vulkan::PipelineOptions::RenderPassOptions::AttachmentType {
	COLOR
};

class Vulkan::PipelineOptions::RenderPassOptions::Attachment {
	public:

		/**
		 * @brief Generates an attachment and his reference based on some standard attachment models.
		 * @details The attachment reference will have predefined layout and his index will always be 0.
		 * 
		 * @param type A set of predefined attachments.
		 * @param format The attachment format can be specified.
		 */
		Attachment(AttachmentType type = AttachmentType::COLOR, VkFormat format = VK_FORMAT_B8G8R8A8_SRGB) : attachment{}, attachmentReference{} {
			//struct for the attachment
			switch (type) {
			case AttachmentType::COLOR:
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
				break;
			}
		}


		Attachment(const VkAttachmentDescription& baseAttachment, VkImageLayout referenceLayout) : attachment{ baseAttachment }, attachmentReference{} {
			attachmentReference.layout = referenceLayout;
		}


		const VkAttachmentDescription& operator+() const {
			return attachment;
		}


		/**
		 * @brief Returns the array of VkAttachmentDescription and the array of VkAttachmentReference.
		 * @details VkAttachmentReference array will have the indexes (VkAttachmentReference.attachment) already set, based on the position of the attachments in the VkAttachmentDescription array.
		 * 
		 * @param ...attachments Attachments to generate the array from.
		 * @tparam A Each argument can be an Attachment or a std::pair<Attachment, VkImageLayout>. The pair is used to pass a different VkAttachmentReference.layout than the one specified during attachment creation.
		 * @return The array of VkAttachmentDescription and the array of VkAttachmentReference.
		 */
		template<IsAttachment... A>
		static std::pair<std::vector<VkAttachmentDescription>, std::vector<VkAttachmentReference>> prepareAttachmentsArray(A... attachments) {
			std::vector<VkAttachmentDescription> attachmentsResult;
			std::vector<VkAttachmentReference> attachmentReferencesResult;

			//Add the attachment and its reference to the corresponding arrays. The right overload is called based on the type of the attachment argument.
			(addAttachment(attachments, attachmentsResult, attachmentReferencesResult), ...);

			return std::pair(attachmentsResult, attachmentReferencesResult);
		}


	private:

		//These 2 functions are used to add an attachment and its reference to the arrays in the prepareAttachmentsArray function.
		static void addAttachment(const std::pair<Attachment, VkImageLayout>& attachment, std::vector<VkAttachmentDescription>& attachments, std::vector<VkAttachmentReference>& attachmentReferences) {
			attachments.push_back(+attachment.first); //add the underlying attachment to the array to return
			VkAttachmentReference attachmentReference{};
			attachmentReference.attachment = attachments.size() - 1; //set the index of the attachment reference
			attachmentReference.layout = attachment.second; //set the layout of the attachment reference grabbing it from the Attachment object
			attachmentReferences.push_back(attachmentReference); //add the attachment reference to the result array
		}

		//These 2 functions are used to add an attachment and its reference to the arrays in the prepareAttachmentsArray function.
		static void addAttachment(const Attachment& attachment, std::vector<VkAttachmentDescription>& attachments, std::vector<VkAttachmentReference>& attachmentReferences) {
			attachments.push_back(+attachment); //add the underlying attachment to the array to return
			VkAttachmentReference attachmentReference{};
			attachmentReference.attachment = attachments.size() - 1; //set the index of the attachment reference
			attachmentReference.layout = attachment.attachmentReference.layout; //set the layout of the attachment reference grabbing it from the second element of the pair argument
			attachmentReferences.push_back(attachmentReference); //add the attachment reference to the result array
		}


		VkAttachmentDescription attachment;
		VkAttachmentReference attachmentReference;
};

#endif
