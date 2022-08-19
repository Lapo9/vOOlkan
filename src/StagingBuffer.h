#ifndef VULKAN_STAGINGBUFFER
#define VULKAN_STAGINGBUFFER

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanException.h"
#include "Buffer.h"


namespace Vulkan::Buffers { class StagingBuffer; }

class Vulkan::Buffers::StagingBuffer : public Buffer {
public:

    StagingBuffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, size_t size) : Buffer(virtualGpu, realGpu, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {

    }



    void fillBuffer(void* data, size_t size) {
        void* rawData;
        vkMapMemory(+virtualGpu, bufferMemory, 0, size, 0, &rawData);
        memcpy(rawData, data, size);
        vkUnmapMemory(+virtualGpu, bufferMemory);
    }


};

#endif
