#ifndef VULKAN_STATICSET
#define VULKAN_STATICSET

#include <glm/glm.hpp>
#include <tuple>
#include <concepts>
#include <type_traits>

#include "UniformBuffer.h"
#include "TextureImage.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Set.h"


namespace Vulkan {

	/**
	 * @brief A DynamicSet is a set of bindings which can be read by the shader.
	 * @details A DynamicSet keeps all the information about its bindings, such as their size, their buffer and the offset in such buffer. This way it is possible to instantiate a DescriptorSet starting from a DynamicSet only.
	 */
	class StaticSet : public Set {
	public:

		struct StaticBufferBindingInfo : public Set::BindingInfo {
			StaticBufferBindingInfo(int size, const Buffers::UniformBuffer& buffer, int offset) : size{ size }, buffer{ buffer }, offset{ offset } {}

			DescriptorSetBindingCreationInfo generateDescriptorSetBindingInfo(unsigned int binding, const VkDescriptorSet& descriptorSet) const override {
				return DescriptorSetBindingCreationInfo{ binding, descriptorSet, size, buffer, offset };
			}

			int size; //the size of the binding
			const Buffers::UniformBuffer& buffer; //reference to the buffer of the binding
			int offset; //offset in his buffer of the binding
		};


		struct ImageBindingInfo : public Set::BindingInfo {
			ImageBindingInfo(const TextureImage& texture) : texture{ texture } {}

			DescriptorSetBindingCreationInfo generateDescriptorSetBindingInfo(unsigned int binding, const VkDescriptorSet& descriptorSet) const override {
				return DescriptorSetBindingCreationInfo{ binding, descriptorSet, texture };
			}

			const TextureImage& texture;
		};



		/**
		 * @brief Creates a StaticSet where each binding is allocated in the corresponding buffer at the corrisponding offset. No checks on the offsets are carried out. Image binding are treated differently.
		 * @details For example, if sizeof(int) = 2 bytes and sizeof(float) = 4 bytes:
		 * @code struct A { int a, b; float c; }; struct B { float d, e; };
		 *		 DynamicSet foo{ {A(), buf1, 0}, {B(), buf2, 0}, {int g(), buf1, 14} };
		 * @details The layout of buf1 will be: aabbcccc------gg where '-' means undefined.
		 *			The layout of buf2 will be: ddddeeeeffff.
		 *			The VkDescriptorSetLayout object will always be a dynamic buffer.
		 *
		 * @param ...bindingsInfo
		 */
		template<typename... Structs, typename... T> requires
			((std::same_as<T, std::tuple<VkShaderStageFlagBits, Structs, Buffers::UniformBuffer*, int>> || std::same_as<T, std::tuple<VkShaderStageFlagBits, TextureImage*>>) && ...)
			StaticSet(const LogicalDevice& virtualGpu, const T&... bindingsInfo) : Set{ virtualGpu } {
			(this->bindingsInfo.push_back(createBindingInfo(bindingsInfo)), ...);

			createDescriptorSetLayout(std::pair{ std::get<0>(bindingsInfo), std::same_as<T, std::tuple<VkShaderStageFlagBits, TextureImage*>> ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }...);
		}



	private:

		template<typename Struct>
		static std::unique_ptr<StaticBufferBindingInfo> createBindingInfo(const std::tuple<VkShaderStageFlagBits, Struct, Buffers::UniformBuffer*, int>& info) {
			return std::make_unique<StaticBufferBindingInfo>(sizeof(std::get<1>(info)), *std::get<2>(info), std::get<3>(info));
		}


		static std::unique_ptr<ImageBindingInfo> createBindingInfo(const std::tuple<VkShaderStageFlagBits, TextureImage*>& info) {
			return std::make_unique<ImageBindingInfo>(*std::get<1>(info));
		}

	};

}



#endif
