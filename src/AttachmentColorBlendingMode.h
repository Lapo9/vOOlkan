#ifndef VULKAN_COLORBLENDER
#define VULKAN_COLORBLENDER

#include <vulkan/vulkan.h>


namespace Vulkan::PipelineOptions::RenderPassOptions { class AttachmentColorBlendingMode; enum class PredefinedColorBlendingModes; }


enum class Vulkan::PipelineOptions::RenderPassOptions::PredefinedColorBlendingModes {
	INVALID, STANDARD //TODO add other modes
};


/**
 * @brief A AttachmentColorBlendingMode describes how a render Subpass will write to a color attachment.
 * @details For example it is possible to specify to override the old color attachment's values, or to mix them.
 * Each AttachmentColorBlendingMode is therefore associated with an attachment in a Subpass.
 */
class Vulkan::PipelineOptions::RenderPassOptions::AttachmentColorBlendingMode {
public:

	AttachmentColorBlendingMode(PredefinedColorBlendingModes predefinedMode= PredefinedColorBlendingModes::INVALID) : colorBlendingMode{} {
		switch (predefinedMode) {
		case PredefinedColorBlendingModes::STANDARD:
			colorBlendingMode.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendingMode.blendEnable = VK_FALSE;
			colorBlendingMode.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendingMode.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendingMode.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendingMode.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendingMode.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendingMode.alphaBlendOp = VK_BLEND_OP_ADD;
			break;
		default:
			colorBlendingMode.colorWriteMask = VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM;
			colorBlendingMode.blendEnable = VK_FALSE;
			colorBlendingMode.srcColorBlendFactor = VK_BLEND_FACTOR_MAX_ENUM;
			colorBlendingMode.dstColorBlendFactor = VK_BLEND_FACTOR_MAX_ENUM;
			colorBlendingMode.colorBlendOp = VK_BLEND_OP_MAX_ENUM;
			colorBlendingMode.srcAlphaBlendFactor = VK_BLEND_FACTOR_MAX_ENUM;
			colorBlendingMode.dstAlphaBlendFactor = VK_BLEND_FACTOR_MAX_ENUM;
			colorBlendingMode.alphaBlendOp = VK_BLEND_OP_MAX_ENUM;
		}
	}


	AttachmentColorBlendingMode(VkPipelineColorBlendAttachmentState baseColorBlendingMode) : colorBlendingMode{ baseColorBlendingMode } {}


	const VkPipelineColorBlendAttachmentState& operator+() const {
		return colorBlendingMode;
	}

private:
	VkPipelineColorBlendAttachmentState colorBlendingMode;
};

#endif