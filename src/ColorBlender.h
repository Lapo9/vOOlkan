#ifndef VULKAN_COLORBLENDER
#define VULKAN_COLORBLENDER

#include <vulkan/vulkan.h>


namespace Vulkan::PipelineOptions { class ColorBlender; enum class ColorBlendMode; }


enum class Vulkan::PipelineOptions::ColorBlendMode {
	INVALID, STANDARD //TODO add othe modes
};


/**
 * @brief A ColorBlender describes how a render Subpass will write to a color attachment.
 * @details For example it is possible to specify to override the old color attachment's values, or to mix them.
 * Each ColorBlender is therefore associated with an attachment in a Subpass.
 */
class Vulkan::PipelineOptions::ColorBlender {
public:

	ColorBlender(ColorBlendMode predefinedMode= ColorBlendMode::INVALID) : colorBlender{} {
		switch (predefinedMode) {
		case ColorBlendMode::STANDARD:
			colorBlender.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlender.blendEnable = VK_FALSE;
			colorBlender.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlender.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlender.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlender.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlender.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlender.alphaBlendOp = VK_BLEND_OP_ADD;
			break;
		default:
			colorBlender.colorWriteMask = VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM;
			colorBlender.blendEnable = VK_FALSE;
			colorBlender.srcColorBlendFactor = VK_BLEND_FACTOR_MAX_ENUM;
			colorBlender.dstColorBlendFactor = VK_BLEND_FACTOR_MAX_ENUM;
			colorBlender.colorBlendOp = VK_BLEND_OP_MAX_ENUM;
			colorBlender.srcAlphaBlendFactor = VK_BLEND_FACTOR_MAX_ENUM;
			colorBlender.dstAlphaBlendFactor = VK_BLEND_FACTOR_MAX_ENUM;
			colorBlender.alphaBlendOp = VK_BLEND_OP_MAX_ENUM;
		}
	}


	ColorBlender(VkPipelineColorBlendAttachmentState baseColorBlender) : colorBlender{ baseColorBlender } {}


	const VkPipelineColorBlendAttachmentState& operator+() const {
		return colorBlender;
	}

private:
	VkPipelineColorBlendAttachmentState colorBlender;
};

#endif