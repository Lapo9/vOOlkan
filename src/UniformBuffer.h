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
	UniformBuffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, size_t size) : Buffer(virtualGpu, realGpu, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {}



	/**
	 * @brief Fills the buffer with the specified data.
	 * @details Each argument vector is padded. The minimum alignment of the GPU used to build this buffer is used. 
	 *			For example if the alignment is 4 and the arguments are {2, 2, 2}, {3, 3} then the buffer will have this layout: {2, 2, 2, 0, 3, 3, 0, 0}.
	 * 
	 * @param ...data floats to insert into the buffer.
	 */
	template<typename D>
	void fillBuffer(const D& data, int offset = 0) {
		void* rawData;
		vkMapMemory(+virtualGpu, bufferMemory, offset, sizeof(D), 0, &rawData);
		memcpy(rawData, &data, sizeof(D));
		vkUnmapMemory(+virtualGpu, bufferMemory);
	}

};

#endif