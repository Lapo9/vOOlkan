#ifndef VULKAN_UNIFORMBUFFER
#define VULKAN_UNIFORMBUFFER

#include <vulkan/vulkan.h>
#include <concepts>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Buffer.h"


namespace Vulkan::Buffers { class UniformBuffer; }


/**
 * @brief This buffer is used to store the uniform data (such as matrices) to be used by the shaders in the GPU.
 */
class Vulkan::Buffers::UniformBuffer : public Buffer {
public:
	UniformBuffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, size_t size) : Buffer(virtualGpu, realGpu, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
		auto limits = realGpu.getProperties();
		alignment = limits.limits.minUniformBufferOffsetAlignment;
	}


	/**
	 * @brief Fills the buffer with the specified data.
	 * @details Each argument vector is padded. The minimum alignment of the GPU used to build this buffer is used. 
	 *			For example if the alignment is 4 and the arguments are {2, 2, 2}, {3, 3} then the buffer will have this layout: {2, 2, 2, 0, 3, 3, 0, 0}.
	 * 
	 * @param ...data floats to insert into the buffer.
	 */
	template<std::same_as<std::vector<float>>... V>
	void fillBuffer(const V&... data) {
		std::vector<float> mergedData;
		([&mergedData, this](const std::vector<float>& data) {
			mergedData.insert(mergedData.end(), data.begin(), data.end());
			int paddingAmount = (alignment - ((mergedData.size() * sizeof(float)) % alignment)) % alignment; //number of padding bytes
			std::vector<float> padding(paddingAmount / sizeof(float), 0.0f); //number of padding floats (usually 1 float = 4 bytes)
			mergedData.insert(mergedData.end(), padding.begin(), padding.end());
			}(data), ...);

		void* rawData;
		vkMapMemory(+virtualGpu, bufferMemory, 0, mergedData.size() * sizeof(mergedData[0]), 0, &rawData);
		memcpy(rawData, mergedData.data(), mergedData.size() * sizeof(mergedData[0]));
		vkUnmapMemory(+virtualGpu, bufferMemory);
	}

	//TODO overload fillBuffer with a function that takes in models and copies all of their uniform data into the buffer following a policy (such as AAAABB AAAABB or AAAA AAAA BB BB) which must be the same as the one choosen in the descriptor set


private:
	int alignment; //used for padding

};

#endif