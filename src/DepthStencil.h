#ifndef VULKAN_DEPTHSTENCIL
#define VULKAN_DEPTHSTENCIL

#include <vulkan/vulkan.h>


namespace Vulkan::PipelineOptions { class DepthStencil; }

//TODO actually fill this class
class Vulkan::PipelineOptions::DepthStencil {
public:

	DepthStencil() : depthStencil{} {
		
	}


	DepthStencil(VkPipelineDepthStencilStateCreateInfo baseDepthStencil) : depthStencil{ baseDepthStencil } {}


	const VkPipelineDepthStencilStateCreateInfo& operator+() const {
		return depthStencil;
	}

private:
	VkPipelineDepthStencilStateCreateInfo depthStencil;
};

#endif