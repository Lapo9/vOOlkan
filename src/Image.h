#ifndef VULKAN_IMAGE
#define VULKAN_IMAGE

#include <vulkan/vulkan.h>
#include <map>
#include <string>

#include "ImageView.h"
#include "LogicalDevice.h"
#include "VulkanException.h"
#include "Buffer.h"

namespace Vulkan { class Image; class LogicalDevice; class PhysicalDevice; class ImageView; namespace SwapchainOptions { class SurfaceFormat; } }

/**
 * @brief An Image is an object representing what can be passed to the swapchain: the content of the image is what will be drawn.
 * @details An Image contains a list of ImageViews where we can actually draw. When an image is created it contains no ImageViews, but you can add those by using the generateImageView function.
 */
class Vulkan::Image {
	public:

		Image(const VkImage& image, const LogicalDevice& virtualGpu, VkFormat format);

		Image(const Image&) = delete;
		Image(Image&&) = default;
		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&) = default;

		const VkImage& operator+() const;

		/**
		 * @brief Returns the format of the image, which is the same format of all the images in the swapchain of this image.
		 * 
		 * @return The format of the image.
		 */
		VkFormat getFormat() const;


		/**
		 * @brief Creates a new image view for this image, and adds it to the array of image views of this image, on last position.
		 * 
		 * @param tag A tag to retrieve this image view.
		 * @param virtualGpu The LogicalGpu of the Vulkan application.
		 * @return The newly created image view.
		 */
		const ImageView& generateImageView(std::string tag, const LogicalDevice& virtualGpu, VkImageAspectFlags type = VK_IMAGE_ASPECT_COLOR_BIT);


		/**
		 * @brief Eliminates the specified image view.
		 * 
		 * @param tag The image view to eliminate.
		 */
		void eliminateImageView(std::string tag);


		/**
		 * @brief Returns the image view associated with the tag.
		 * @details Throws a VulkanException if the tag is not existent.
		 * 
		 * @param tag The tag of the image view to retrieve.
		 * @return The image view associated with the tag.
		 */
		const ImageView& operator[](std::string tag) const;


		/**
		 * @brief Returns all the image views of this image.
		 * 
		 * @return All the image views of this image.
		 */
		const std::map<std::string, ImageView>& getImageViews() const;

	protected:
		//used by child classes only (they also need a custom dtor)
		template<std::same_as<VkMemoryPropertyFlagBits>... P>
		Image(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, VkFormat format, std::pair<unsigned int, unsigned int> resolution, VkImageTiling tiling, VkImageUsageFlags usage, P... memoryProperties) : format{ format } {
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = resolution.first;
			imageInfo.extent.height = resolution.second;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = tiling;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usage;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (VkResult result = vkCreateImage(+virtualGpu, &imageInfo, nullptr, &image); result != VK_SUCCESS) {
				throw VulkanException("Failed to create image!", result);
			}

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(+virtualGpu, image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = Buffers::Buffer::findSuitableMemoryIndex(realGpu, memRequirements.memoryTypeBits, memoryProperties...);

			if (VkResult result = vkAllocateMemory(+virtualGpu, &allocInfo, nullptr, &allocatedMemory); result != VK_SUCCESS) {
				throw VulkanException("Failed to allocate image memory!", result);
			}

			vkBindImageMemory(+virtualGpu, image, allocatedMemory, 0);
		}
		
		
		VkImage image;
		VkFormat format;
		std::map<std::string, ImageView> views;
		VkDeviceMemory allocatedMemory;
};

#endif

