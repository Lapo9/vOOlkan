#ifndef VULKAN_IMAGE
#define VULKAN_IMAGE

#include <vulkan/vulkan.h>
#include <map>
#include <string>

#include "SwapchainSurfaceFormat.h"
#include "LogicalDevice.h"
#include "ImageView.h"


namespace Vulkan { class Image; }

/**
 * @brief An Image is an object representing what can be passed to the swapchain: the content of the image is what will be drawn.
 * @details An Image contains a list of ImageViews where we can actually draw. When an image is created it contains no ImageViews, but you can add those by using the generateImageView function.
 */
class Vulkan::Image {
	public:

		Image(const VkImage& image, const SwapchainOptions::SurfaceFormat& format) : image{ image }, format{ format } {
		}

		const VkImage& operator+() const {
			return image;
		}

		/**
		 * @brief Returns the format of the image, which is the same format of all the images in the swapchain of this image.
		 * 
		 * @return The format of the image.
		 */
		const SwapchainOptions::SurfaceFormat& getFormat() const {
			return format;
		}


		/**
		 * @brief Creates a new image view for this image, and adds it to the array of image views of this image, on last position.
		 * 
		 * @param tag A tag to retrieve this image view.
		 * @param virtualGpu The LogicalGpu of the Vulkan application.
		 * @return The newly created image view.
		 */
		ImageView& generateImageView(std::string tag, const LogicalDevice& virtualGpu) {
			auto res = views.emplace(std::piecewise_construct,
				std::forward_as_tuple(tag),
				std::forward_as_tuple(image, virtualGpu));

			if (!res.second) {
				throw VulkanException("Cannot create an image view with tag " + tag + " because the tag has been already used");
			}
			return views[tag];
		}


		/**
		 * @brief Returns the image view associated with the tag.
		 * @details Throws a VulkanException if the tag is not existent.
		 * 
		 * @param tag The tag of the image view to retrieve.
		 * @return The image view associated with the tag.
		 */
		ImageView& operator[](std::string tag) {
			if (views.find(tag) == views.end()) {
				throw VulkanException("Image view with tag " + tag + " doesn't exist");
			}
			return views[tag];
		}

	private:
		VkImage image;
		const SwapchainOptions::SurfaceFormat& format;
		std::map<std::string, ImageView> views;
};

#endif

