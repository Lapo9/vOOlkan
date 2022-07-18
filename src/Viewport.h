#ifndef VULKAN_VIEWPORT
#define VULKAN_VIEWPORT

#include "vulkan/vulkan.h"


namespace Vulkan::PipelineOptions { class Viewport; }

/**
 * @brief This class is used to tell the pipeline that 1 dynamic viewport is present.
 * @details At the moment only a single dynamic viewport and scissor is possible.
 */
class Vulkan::PipelineOptions::Viewport {
public:

	Viewport() : viewport{} {
		viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport.viewportCount = 1;
		viewport.scissorCount = 1;
	}

	const VkPipelineViewportStateCreateInfo& operator+() const {
		return viewport;
	}

private:
	VkPipelineViewportStateCreateInfo viewport;
};

#endif
