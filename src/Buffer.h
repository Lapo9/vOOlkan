#ifndef VULKAN_BUFFER
#define VULKAN_BUFFER

#include <vulkan/vulkan.h>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanException.h"
#include "VertexInput.h"


namespace Vulkan { class Buffer; }

class Vulkan::Buffer {
public:
    
    //DEBUG of course one should be able to add the data he wants to the buffer
    using Vertex = PipelineOptions::Vertex<glm::vec2, glm::vec3>;
    std::vector<Vertex> model{
        {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };



    Buffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu) : virtualGpu{ virtualGpu } {
        //create the buffer
        createBuffer(virtualGpu);

        //find out the constraints about the real GPU memory to use for this buffer
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(+virtualGpu, buffer, &memRequirements);

        //find which GPU memory to use
        auto memIndex = findSuitableMemoryIndex(realGpu, memRequirements.memoryTypeBits);

        //allocate the choosen memory
        allocateMemory(memIndex, memRequirements.size);

        //DEBUG
        fillGpuMemory(sizeof(model[0]) * model.size());

        //associate the allocated memory with the created buffer
        vkBindBufferMemory(+virtualGpu, buffer, bufferMemory, 0);
	}


    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) = default; //TODO
    Buffer& operator=(Buffer&&) = default; //TODO


    ~Buffer() {
         vkDestroyBuffer(+virtualGpu, buffer, nullptr);
         vkFreeMemory(+virtualGpu, bufferMemory, nullptr);
    }


    const VkBuffer& operator+() const {
        return buffer;
    }


    void fillBuffer() {

    }


private:

    //Create the buffer
    void createBuffer(const LogicalDevice& virtualGpu) {    
        //info about the buffer we want to create
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(model[0]) * model.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        //actually create the buffer
        if (VkResult result = vkCreateBuffer(+virtualGpu, &bufferInfo, nullptr, &buffer); result != VK_SUCCESS) {
            throw VulkanException("Failed to create buffer!", result);
        }
    }


    //Return the first index of a memory type on the GPU which is suitable for the resource we want to load onto the GPU. 
    uint32_t findSuitableMemoryIndex(const PhysicalDevice& realGpu, uint32_t suitableTypesBitmask) const {
        VkMemoryPropertyFlags requiredMemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        //get the properties of the memory of the GPU
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(+realGpu, &memProperties); 

        //for each memory type, check if it is suitable for what we want to do (i.e. this type is 1 in the suitableTypesBitmask and the properties of this type are a superset of requiredMemoryProperties)
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((suitableTypesBitmask & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & requiredMemoryProperties) == requiredMemoryProperties) {
                return i;
            }
        }

        throw VulkanException{ "No suitable memory for the buffer" };
    }


    //Allocate the required space on the GPU
    void allocateMemory(uint32_t memoryIndex, VkDeviceSize sizeToAllocate) {
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = sizeToAllocate;
        allocInfo.memoryTypeIndex = memoryIndex;

        if (VkResult result = vkAllocateMemory(+virtualGpu, &allocInfo, nullptr, &bufferMemory); result != VK_SUCCESS) {
            throw VulkanException{ "Failed to allocate buffer memory!", result };
        }
    }


    //Fill the GPU RAM with the specified data from the CPU RAM
    void fillGpuMemory(size_t size) {
        void* data;
        vkMapMemory(+virtualGpu, bufferMemory, 0, size, 0, &data); //map GPU memory to CPU memory
        memcpy(data, model.data(), size); //copy required data
        vkUnmapMemory(+virtualGpu, bufferMemory); //unmap GPU memory from CPU memory (data is already there)
    }


	VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    const LogicalDevice& virtualGpu;

};

#endif
