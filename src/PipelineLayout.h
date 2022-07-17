#ifndef VULKAN_PIPELINELAYOUT
#define VULKAN_PIPELINELAYOUT

#include <vulkan/vulkan.h>


namespace Vulkan {
	class LogicalDevice;
	namespace PipelineOptions { class PipelineLayout; }
}


/**
 * @brief This objects is a reference to all of the resources (e.g. uniforms) that the pipeline can access.
 */
class Vulkan::PipelineOptions::PipelineLayout {
public:

	PipelineLayout(const LogicalDevice& virtualGpu);

	PipelineLayout(const PipelineLayout&) = delete;
	PipelineLayout(PipelineLayout&&) = delete;
	PipelineLayout& operator=(const PipelineLayout&) = delete;
	PipelineLayout& operator=(PipelineLayout&&) = delete;

	~PipelineLayout();


	const VkPipelineLayout operator+() const;

private:
	VkPipelineLayout pipelineLayout;
	const LogicalDevice& virtualGpu;
};


#endif
