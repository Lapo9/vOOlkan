#ifndef VULKAN_VERTEXBUFFER
#define VULKAN_VERTEXBUFFER

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanException.h"
#include "VertexInput.h"
#include "Buffer.h"
#include "Model.h"


namespace Vulkan::Buffers { class VertexBuffer; }

class Vulkan::Buffers::VertexBuffer : public Buffer{
public:

    VertexBuffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, size_t size) : Buffer(virtualGpu, realGpu, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        
	}


    //IMPROVE I should save the offset of each different model, so that I can use these offsets during the call to vkCmdBindVertices in the drawer (and for PipelineVertexArrays)
    template<typename... C, template<typename...> class... M> requires (std::same_as<M<C...>, Model<C...>> && ...)
    void fillBuffer(const M<C...>&... models) {
        std::vector<PipelineOptions::Vertex<C...>> data{};
        (data.insert(data.end(), models.getVertices().begin(), models.getVertices().end()), ...); //copy all the vertices to one vector
      
        verticesCount = data.size();

        //TODO staging buffer

        //copy data to vertex buffer
        void* rawData;
        vkMapMemory(+virtualGpu, bufferMemory, 0, data.size() * sizeof(data[0]), 0, &rawData);
        memcpy(rawData, data.data(), data.size() * sizeof(data[0]));
        vkUnmapMemory(+virtualGpu, bufferMemory);
    }


    unsigned int getVerticesCount() const {
        return verticesCount;
    }


private:

    unsigned int verticesCount;
};

#endif
