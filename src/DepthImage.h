#ifndef VULKAN_DEPTHIMAGE
#define VULKAN_DEPTHIMAGE

#include <vulkan/vulkan.h>

#include "Image.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"


namespace Vulkan { class DepthImage; }

class Vulkan::DepthImage : public Vulkan::Image {
public:
	DepthImage(const LogicalDevice& virtualGpu, const PhysicalDevice& realGpu, std::pair<unsigned int, unsigned int> resolution)
		: Image{ virtualGpu, realGpu, VK_FORMAT_D32_SFLOAT, resolution, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
	{
		generateImageView("base", virtualGpu, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	DepthImage(const DepthImage&) = delete;
	DepthImage& operator=(const DepthImage&) = delete;
	DepthImage(DepthImage&&) = default;
	DepthImage& operator=(DepthImage&& movedFrom) = default;

	~DepthImage() = default;
};



#endif
