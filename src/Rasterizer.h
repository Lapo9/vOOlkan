#ifndef VULKAN_RASTERIZER
#define VULKAN_RASTERIZER

#include <vulkan/vulkan.h>


namespace Vulkan::PipelineOptions { class Rasterizer; }

/**
 * @brief Describes how the vertices will be transformed to fragments to be drawn on screen.
 * @details Some of the most notable options are: back face culling, polygon fill/wireframe, lines width, ...
 */
class Vulkan::PipelineOptions::Rasterizer {
public:
	//TODO add creation options
	Rasterizer() : rasterizer{} {
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;
	}


	Rasterizer(VkPipelineRasterizationStateCreateInfo baseRasterizer) : rasterizer{ baseRasterizer } {}


	const VkPipelineRasterizationStateCreateInfo& operator+() const {
		return rasterizer;
	}

private:
	VkPipelineRasterizationStateCreateInfo rasterizer;
};

#endif
