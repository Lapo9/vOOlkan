#ifndef VULKAN_UNIFORMBUFFER
#define VULKAN_UNIFORMBUFFER

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Buffer.h"


namespace Vulkan::Buffers { class UniformBuffer; }

class Vulkan::Buffers::UniformBuffer : public Buffer {
public:
	UniformBuffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, size_t size) : Buffer(virtualGpu, realGpu, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {

	}

	//TODO fill buffer
};

#endif