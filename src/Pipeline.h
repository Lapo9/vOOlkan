#ifndef VULKAN_PIPELINE
#define VULKAN_PIPELINE

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <concepts>

#include "LogicalDevice.h"
#include "Attachment.h"
#include "AttachmentColorBlendingMode.h"
#include "DepthStencil.h"
#include "DynamicState.h"
#include "InputAssembly.h"
#include "Multisampler.h"
#include "PipelineColorBlendingModes.h"
#include "PipelineLayout.h"
#include "Rasterizer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Subpass.h"
#include "VertexInput.h"
#include "Viewport.h"


namespace Vulkan { class Pipeline; }

class Vulkan::Pipeline {

public:
	
	Pipeline(
		const LogicalDevice& virtualGpu,
		const PipelineOptions::RenderPass& renderPass,
		int subpassIndex,
		const std::vector<PipelineOptions::Shader*>& shaders,	
		const PipelineOptions::PipelineVertexArrays& vertexArraysDescriptor,
		const PipelineOptions::PipelineLayout& pipelineLayout,
		const PipelineOptions::InputAssembly& inputAssembly = PipelineOptions::InputAssembly{},
		const PipelineOptions::Rasterizer& rasterizer = PipelineOptions::Rasterizer{},
		const PipelineOptions::Multisampler& multisampler = PipelineOptions::Multisampler{},
		const PipelineOptions::DepthStencil& depthStencil = PipelineOptions::DepthStencil{},
		const PipelineOptions::DynamicState& dynamicState = PipelineOptions::DynamicState{},
		const PipelineOptions::Viewport& viewport = PipelineOptions::Viewport{}
	) : virtualGpu{ virtualGpu }, pipelineLayout{ pipelineLayout } {
		//create the array of VkPipelineShaderStageCreateInfo starting from the shaders
		std::vector<VkPipelineShaderStageCreateInfo> shadersDescriptors;
		for (const auto& shader : shaders) {
			shadersDescriptors.push_back(+*shader);
		}

		//create the PipelineColorBlendingModes object starting from the subpass
		PipelineOptions::PipelineColorBlendingModes pipelineColorBlendingModes{ renderPass.getSubpass(subpassIndex) };


		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = nullptr;
		pipelineInfo.stageCount = shadersDescriptors.size();
		pipelineInfo.pStages = shadersDescriptors.data();
		pipelineInfo.pVertexInputState = &+vertexArraysDescriptor;
		pipelineInfo.pInputAssemblyState = &+inputAssembly;
		pipelineInfo.pViewportState = &+viewport;
		pipelineInfo.pRasterizationState = &+rasterizer;
		pipelineInfo.pMultisampleState = &+multisampler;
		pipelineInfo.pDepthStencilState = &+depthStencil;
		pipelineInfo.pColorBlendState = &+pipelineColorBlendingModes;
		pipelineInfo.pDynamicState = &+dynamicState;
		pipelineInfo.layout = +pipelineLayout;
		pipelineInfo.renderPass = +renderPass;
		pipelineInfo.subpass = subpassIndex;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (VkResult result = vkCreateGraphicsPipelines(+virtualGpu, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline); result != VK_SUCCESS) {
			throw VulkanException("Failed to create graphics pipeline!", result);
		}
	}

	Pipeline(const Pipeline&&) = delete;
	Pipeline(Pipeline&&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline& operator=(Pipeline&&) = delete;

	~Pipeline() {
		vkDestroyPipeline(+virtualGpu, pipeline, nullptr);
	}

	const VkPipeline& operator+() const {
		return pipeline;
	}


	const PipelineOptions::PipelineLayout& getLayout() const {
		return pipelineLayout;
	}

private:
	VkPipeline pipeline;
	const LogicalDevice& virtualGpu;
	const PipelineOptions::PipelineLayout& pipelineLayout;
};


#endif
