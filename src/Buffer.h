#ifndef VULKAN_BUFFER
#define VULKAN_BUFFER

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanException.h"


namespace Vulkan::Buffers { class Buffer; }

class Vulkan::Buffers::Buffer {
public:

    template<std::same_as<VkMemoryPropertyFlagBits>... P>
    Buffer(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, size_t size, int usage, P... requiredMemoryProperties) : virtualGpu{ virtualGpu }, size{ size } {
        //create the buffer
        createBuffer(virtualGpu, size, usage);

        //find out the constraints about the real GPU memory to use for this buffer
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(+virtualGpu, buffer, &memRequirements);

        //find which GPU memory to use
        auto memIndex = findSuitableMemoryIndex(realGpu, memRequirements.memoryTypeBits, requiredMemoryProperties...);

        //allocate the choosen memory
        allocateMemory(memIndex, memRequirements.size);

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


    VkDeviceMemory& getBufferMemory() {
        return bufferMemory;
    }


    //Return the first index of a memory type on the GPU which is suitable for the resource we want to load onto the GPU. 
    template<std::same_as< VkMemoryPropertyFlagBits>... P>
    static uint32_t findSuitableMemoryIndex(const PhysicalDevice& realGpu, uint32_t suitableTypesBitmask, P... requiredMemoryProperties) {
        //get the properties of the memory of the GPU
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(+realGpu, &memProperties);
        auto requiredMemoryPropertiesMask = (requiredMemoryProperties | ...);

        //for each memory type, check if it is suitable for what we want to do (i.e. this type is 1 in the suitableTypesBitmask and the properties of this type are a superset of requiredMemoryProperties)
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((suitableTypesBitmask & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & requiredMemoryPropertiesMask) == requiredMemoryPropertiesMask) {
                return i;
            }
        }

        throw VulkanException{ "No suitable memory for the buffer" };
    }


protected:

    //Create the buffer
    void createBuffer(const LogicalDevice& virtualGpu, size_t size, int usage) {
        //info about the buffer we want to create
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        //actually create the buffer
        if (VkResult result = vkCreateBuffer(+virtualGpu, &bufferInfo, nullptr, &buffer); result != VK_SUCCESS) {
            throw VulkanException("Failed to create buffer!", result);
        }
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


    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    const LogicalDevice& virtualGpu;
    const size_t size;
};


#endif
