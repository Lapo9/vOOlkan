#ifndef VULKAN_IMAGEVIEW
#define VULKAN_IMAGEVIEW

#include <vulkan/vulkan.h>


namespace Vulkan { class ImageView; class LogicalDevice; class Image; }

/**
 * @brief Part of an image where we can actually draw.
 */
class Vulkan::ImageView {
public:

	ImageView(const Image& image, const LogicalDevice& virtualGpu, VkImageAspectFlags type = VK_IMAGE_ASPECT_COLOR_BIT);

	~ImageView();

	ImageView(const ImageView&) = delete;
	ImageView(ImageView&&) = default;
	ImageView& operator=(const ImageView&) = delete;
	ImageView& operator=(ImageView&&) = default;

	const VkImageView& operator+() const;

private:
	VkImageView imageView;
	const LogicalDevice& virtualGpu;
};

#endif
