#ifndef VULKAN_DYNAMICSTATE
#define VULKAN_DYNAMICSTATE

#include <vulkan/vulkan.h>
#include <vector>
#include <concepts>
#include <set>
#include <iterator>


namespace Vulkan::PipelineOptions { class DynamicState; }


/**
 * @brief Describes what can be changed in the pipeline without having to create a new pipeline. Viewport and scissors are always dynamic.
 */
class Vulkan::PipelineOptions::DynamicState {
public:

	template<typename... DS> requires (std::same_as<DS, VkDynamicState> && ...)
		DynamicState(DS... dynamicStates) : dynamicState{}, usedDynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } {
		std::set<VkDynamicState> enumerateDynamicStates;
		(enumerateDynamicStates.insert(dynamicStates), ...); //put the dynamic states into a set in order to avoid duplicates
		std::copy(enumerateDynamicStates.begin(), enumerateDynamicStates.end(), std::back_inserter(usedDynamicStates)); //copy the set into the vector

		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(usedDynamicStates.size());
		dynamicState.pDynamicStates = usedDynamicStates.data();
	}


	const VkPipelineDynamicStateCreateInfo& operator+() const {
		return dynamicState;
	}


private:
	VkPipelineDynamicStateCreateInfo dynamicState;
	std::vector<VkDynamicState> usedDynamicStates;
};

#endif
