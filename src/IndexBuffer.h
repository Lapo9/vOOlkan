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
        
        unsigned int currentOffset = 0;
        ([this, &currentOffset, &data](const M<C...>& model) {
            //copy all the vertices to one vector
            for (auto index : model.getIndexes()) {
                data.push_back(index + currentOffset);
            }
            
            modelOffsets.push_back(currentOffset); //save the offset for this model
            currentOffset += model.getIndexes().size(); //increase next offset
        }(models), ...);
        modelOffsets.push_back(currentOffset); //save the last offset (= indexesCount) in order to speed up the getModelIndexesCount function
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


    unsigned int getModelOffset(unsigned int i) const {
        return modelOffsets[i];
    }


    unsigned int getModelIndexesCount(unsigned int i) const {
        return modelOffsets[i + 1] - modelOffsets[i];
    }


    unsigned int getModelsCount() const {
        return modelOffsets.size() - 1;
    }


private:

    unsigned int indexesCount;
    std::vector<unsigned int> modelOffsets;
};

#endif
