#ifndef VULKAN_INDEXBUFFER
#define VULKAN_INDEXBUFFER

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanException.h"
#include "Buffer.h"
#include "Model.h"


namespace Vulkan::Buffers { class IndexBuffer; }

class Vulkan::Buffers::IndexBuffer : public Buffer {
public:

    IndexBuffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, size_t size) : Buffer(virtualGpu, realGpu, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {

    }


    template<typename... C, template<typename...> class... M> requires (std::same_as<M<C...>, Model<C...>> && ...)
        void fillBuffer(const M<C...>&... models) {
        std::vector<uint32_t> data{};
        (data.insert(data.end(), models.getIndexes().begin(), models.getIndexes().end()), ...); //copy all the vertices to one vector

        indexesCount = data.size();

        //TODO staging buffer

        //copy data to vertex buffer
        void* rawData;
        vkMapMemory(+virtualGpu, bufferMemory, 0, data.size() * sizeof(data[0]), 0, &rawData);
        memcpy(rawData, data.data(), data.size() * sizeof(data[0]));
        vkUnmapMemory(+virtualGpu, bufferMemory);
    }


    unsigned int getIndexesCount() const {
        return indexesCount;
    }


private:

    unsigned int indexesCount;
};

#endif
