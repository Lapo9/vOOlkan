#ifndef VULKAN_DEPTHSTENCIL
#define VULKAN_DEPTHSTENCIL

#include <vulkan/vulkan.h>


namespace Vulkan::PipelineOptions { class DepthStencil; }

//TODO actually fill this class
class Vulkan::PipelineOptions::DepthStencil {
public:

	DepthStencil() : depthStencil{} {
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};
	}


	DepthStencil(VkPipelineDepthStencilStateCreateInfo baseDepthStencil) : depthStencil{ baseDepthStencil } {}


	const VkPipelineDepthStencilStateCreateInfo& operator+() const {
		return depthStencil;
	}

private:
	VkPipelineDepthStencilStateCreateInfo depthStencil;
};

#endif