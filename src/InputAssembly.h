#ifndef VULKAN_INPUTASSEMBLY
#define VULKAN_INPUTASSEMBLY

#include "vulkan/vulkan.h"

namespace Vulkan::PipelineOptions { class InputAssembly; }

/**
 * @brief Describes how the vertices will be connected. e.g. triangle list, triangle fan, ...
 */
class Vulkan::PipelineOptions::InputAssembly {
public:
	//TODO add options
	InputAssembly() : inputAssembly{} {
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
	}


	InputAssembly(VkPipelineInputAssemblyStateCreateInfo baseInputAssembly) : inputAssembly{ baseInputAssembly } {}


	const VkPipelineInputAssemblyStateCreateInfo& operator+() const {
		return inputAssembly;
	}


private:
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
};


#endif
