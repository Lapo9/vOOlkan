#ifndef VULKAN_UNIFORMBUFFER
#define VULKAN_UNIFORMBUFFER

#include <vulkan/vulkan.h>
#include <concepts>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Buffer.h"


namespace Vulkan::Buffers { class UniformBuffer; }

class Vulkan::Buffers::UniformBuffer : public Buffer {
public:
	UniformBuffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, size_t size) : Buffer(virtualGpu, realGpu, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {

	}


	//DEBUG this function just fill the buffer, but it is not very useful (e.g. padding, ...)
	void fillBuffer(const std::vector<float>& data) {
		void* rawData;
		vkMapMemory(+virtualGpu, bufferMemory, 0, data.size() * sizeof(data[0]), 0, &rawData);
		memcpy(rawData, data.data(), data.size() * sizeof(data[0]));
		vkUnmapMemory(+virtualGpu, bufferMemory);
	}

	//TODO overload fillBuffer with a function that takes in models and copies all of their uniform data into the buffer following a policy (such as AAAABB AAAABB or AAAA AAAA BB BB) which must be the same as the one choosen in the descriptor set
};

#endif