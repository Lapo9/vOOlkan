#ifndef VULKAN_PIPELINECOLORBLENDINGMODES
#define VULKAN_PIPELINECOLORBLENDINGMODES

#include <vulkan/vulkan.h>
#include "Subpass.h"


namespace Vulkan::PipelineOptions { class PipelineColorBlendingModes; }

/**
 * @brief A descriptor which unifies all the AttachmentColorBlendingMode of the same subpass into a structure which can be used to create to pipeline.
 */
class Vulkan::PipelineOptions::PipelineColorBlendingModes {
public:
	PipelineColorBlendingModes(const RenderPassOptions::Subpass& subpass, float blendCostantR, float blendCostantG, float blendCostantB, float blendCostantA) : pipelineColorBlendingModes{} {
		pipelineColorBlendingModes.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendingModes.logicOpEnable = VK_FALSE;
		pipelineColorBlendingModes.logicOp = VK_LOGIC_OP_COPY;
		pipelineColorBlendingModes.attachmentCount = subpass.getColorBlendingDescriptors().size();
		pipelineColorBlendingModes.pAttachments = subpass.getColorBlendingDescriptors().data();
		pipelineColorBlendingModes.blendConstants[0] = blendCostantR;
		pipelineColorBlendingModes.blendConstants[1] = blendCostantG;
		pipelineColorBlendingModes.blendConstants[2] = blendCostantB;
		pipelineColorBlendingModes.blendConstants[3] = blendCostantA;
	}

	const VkPipelineColorBlendStateCreateInfo& operator+() const {
		return pipelineColorBlendingModes;
	}

private:
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendingModes;
};

#endif
